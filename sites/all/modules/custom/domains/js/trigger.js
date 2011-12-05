(function ($) {
  Drupal.behaviors.domainsTriggerAJAX = {
    attach: function (context) {
      $('#domains-register-form .form-text', context).bind('keypress', function (e) {
        if(e.which == 13) {
          $('#domains-register-form #edit-submit', context).trigger('mousedown');
        }
      });
    }
  }
})(jQuery);