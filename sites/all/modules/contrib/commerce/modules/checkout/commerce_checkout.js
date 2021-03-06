;(function($) {

  /**
   * Disable the continue buttons in the checkout process once they are clicked
   * and provide a notification to the user.
   */
  Drupal.behaviors.commerceCheckout = {
    attach: function (context, settings) {
      // When the buttons to move from page to page in the checkout process are
      // clicked we disable them so they are not accidently clicked twice.
      $('input.checkout-continue', context).click(function() {
        var $this = $(this);
        $this.clone().insertAfter(this).attr('disabled', true).after(Drupal.theme('checkoutProcessing'));
        $this.hide();
      });
    }
  }

  /**
   * Theme function for checkout button clicked processing indication.
   */
  Drupal.theme.prototype.checkoutProcessing = function() {
    return '<span class="checkout-processing"></span>';
  }
})(jQuery);