<?php

/**
 * @file
 * Allow users to create a taxonomy term and use it automatically for content.
 *
 * Management and form functions only required on menu pages.
 */

/**
 * Menu callback and dispatcher.
 *
 * Based on the current status of the user and premium_tags, different options are displayed:
 * 1) If the user is not associated with any premium_tags tids, then they are given the ability to create one.
 * 2) If the vocabulary is set up for mulitple values, and the user can create an additional term, they are
 *   allowed to review their available tags.
 * 3) if there is already a term for this user, then we allow them to edit that term.
 *
 */
function _premium_tags_user_tags($account) {
  $vid = variable_get('premium_tags_vocabulary', FALSE);
  $vocabulary = taxonomy_vocabulary_load($vid);
  $tids = premium_tags_tids($account->uid);
  if (empty($tids)) {
    return drupal_get_form('premium_tags_term_form', $account);
  }
  elseif (user_access('create additional terms')) {
    return _premium_tags_overview($account, $tids);
  }
  else {
    $term = taxonomy_term_load(array_pop($tids));
    return drupal_get_form('premium_tags_term_form', $account, $term);
  }
}

/**
 * This is the information that is displayed if a user can create additional terms and is viewing their current terms.
 */
function _premium_tags_overview($user, $tids) {
  $output = '<h3>Your Tags</h3>';
  $terms = array();

  foreach ($tids as $tid) {
    $term = taxonomy_term_load($tid);
    $terms[] = array(
      'title' => $term->name,
      'href' => 'user/' . $user->uid . '/tags/edit/' . $tid,
    );
  }
  $output .= theme('links', array('links' => $terms));

  return $output;
}

/**
 * This is the term form that allows you to create or edit your tags name and description.
 * These are values that get written to term_data.
 */
function premium_tags_term_form($form, &$form_state, $user, $term = array()) {
  $vid = variable_get('premium_tags_vocabulary', FALSE);
  $vocabulary = taxonomy_vocabulary_load($vid);
  $form['#term'] = $term;
  $form['#vocabulary'] = (array) $vocabulary;

  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Tag name'),
    '#default_value' => isset($term->name) ? $term->name : '',
    '#maxlength' => 255,
    '#description' => t('The name of your tag.'),
    '#required' => TRUE,
  );
  $form['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#default_value' => !empty($term->description) ? $term->description : '',
    '#description' => t('A description of your tag.'),
  );

  $form['uid'] = array(
    '#type' => 'value',
    '#value' => $user->uid,
  );
  $form['vid'] = array(
    '#type' => 'value',
    '#value' => $vocabulary->vid,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => 100,
  );

  if (!empty($term->tid)) {
    if (user_access('delete own term')) {
      $form['delete'] = array(
        '#value' => l(t('Delete'), 'user/' . $user->uid . '/tags/delete/' . $term->tid),
        '#weight' => 101,
      );
    }
    $form['tid'] = array(
      '#type' => 'value',
      '#value' => $term->tid,
    );
  }
  $form['#redirect'] = 'user/' . $user->uid;
  return $form;
}

/**
 * Process submitted term add/edit form.
 */
function premium_tags_term_form_submit($form, &$form_state) {
  if (empty($form_state['values']['tid'])) {
    $term = new stdClass();
  }
  else {
    $term = taxonomy_term_load($form_state['values']['tid']);
  }
  $term->name = $form_state['values']['name'];
  $term->description = $form_state['values']['description'];
  $term->vid = $form_state['values']['vid'];
  switch (taxonomy_term_save($term)) {
    case SAVED_NEW:
      if (premium_tags_create($form_state['values']['uid'], $term->tid) == SAVED_NEW) {
        drupal_set_message(t('%term created.', array('%term' => $term->name)));
      }
      break;
    case SAVED_UPDATED:
      drupal_set_message(t('%term updated.', array('%term' => $term->name)));
      break;
  }
}

/**
 * Form for deleting association.
 */
function premium_tags_term_delete($form, &$form_state, $user, $term) {
  $form['uid'] = array(
    '#type' => 'value',
    '#value' => $user->uid,
  );
  $form['type'] = array(
    '#type' => 'value',
    '#value' => 'term',
  );
  $form['name'] = array(
    '#type' => 'value',
    '#value' => $term->name,
  );
  $form['tid'] = array(
    '#type' => 'value',
    '#value' => $term->tid,
  );
  $form['delete'] = array(
    '#type' => 'value',
    '#value' => TRUE,
  );
  return confirm_form($form,
                  t('Are you sure you want to delete %title?',
                  array('%title' => $term->name)),
                  'user/' . $user->uid,
                  t('This action cannot be undone.'),
                  t('Delete'),
                  t('Cancel'));
}

/**
 * Process submitted term confirm form deletion.
 */
function premium_tags_term_delete_submit($form, &$form_state) {
  if ($form_state['values']['delete'] === TRUE) {
    taxonomy_term_delete($form_state['values']['tid']);
    taxonomy_check_vocabulary_hierarchy($form['#vocabulary'], $form_state['values']);
    drupal_set_message(t('Deleted %name.', array('%name' => $form_state['values']['name'])));
    watchdog('taxonomy', 'Deleted tag %name.', array('%name' => $form_state['values']['name']), WATCHDOG_NOTICE);
    $form_state['redirect'] = 'user/' . $form_state['values']['uid'];
    // Remove association.
    premium_tags_delete($form_state['values']['uid'], $form_state['values']['tid']);
  }
}

/**
 * Return tid from terms not associated with users.
 */
function _premium_tags_unassociated($terms) {
  $sql = "SELECT uid FROM {premium_tags} WHERE tid = :tid";
  foreach ($terms as $term) {
    $data = db_query($sql, array(':tid' => $term->tid))->fetch();
    if ($data === FALSE) {
      // No uid for this term.
      return $term->tid;
    }
  }
  return FALSE;
}
