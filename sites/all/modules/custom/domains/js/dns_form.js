(function ($) {
  Drupal.behaviors.domainsDnsForm = {
    attach: function (context) {
      // Highlight row when there are changed.
      $('#domains-domain-dns-records-form', context).find('tr input, tr select').change(function() {
         $(this).parents('tr').eq(0).addClass('selected'); 
      });
    }
  }
})(jQuery);