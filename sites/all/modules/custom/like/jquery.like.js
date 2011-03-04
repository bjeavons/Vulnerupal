(function ($) {
  // Documentation on Drupal JavaScript behaviors can be found here: http://drupal.org/node/114774#javascript-behaviors
  Drupal.behaviors.like = {
    attach: function(context){
      jQuery('.'+ Drupal.settings.like.widget_class +':not(.like-processed)', context).addClass('like-processed').each(function(){
        var like_widget = jQuery(this);
        like_widget.find('.'+ Drupal.settings.like.link_class).attr('href', function(){ return jQuery(this).attr('href') + '?json=true'; }).click(function(){
          jQuery.getJSON(jQuery(this).attr('href'), function(json){
            like_widget.find('.'+ Drupal.settings.like.score_class).hide().fadeIn('slow').html(json.score);
            like_widget.find('.'+ Drupal.settings.like.message_class).html(json.voted);
          });
          // Preventing the /like/vote/<nid> target from being triggered.
          return false;
        });
      });
    }
  };
})(jQuery)