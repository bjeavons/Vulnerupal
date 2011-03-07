  <div class="<?php echo $variables['like']['widget_class'];?>">
    <div class="<?php echo $variables['like']['score_class'];?>">
      <?php echo $score; ?>
    </div>
  <?php if (!$variables['logged_in'] || user_access('like content')) { ?>
      <div class="<?php echo $variables['like']['message_class'];?>">
  <?php if (!$variables['logged_in'] && !user_access('like content')) { ?>
      <small> <?php echo $variables['like']['loginvote'];?> </small>
   <?php }
    elseif ($variables['voted']) { ?>
      <?php echo $variables['like']['youvoted'];
    }
    elseif (user_access('like content')) {
      /* The class name provided by Drupal.settings.like.link_class what we will search for in our jQuery later. */
     ?>
     <div class="like-vote"> <?php echo $variables['like']['linkvote'];?> </div>
   <?php } ?>
    </div>
  <?php } ?>
  </div>