<?php

/**
 * @file
 * 
 * Enom Pages
 * Group of enom page callbacks
 * 
 */

/**
 * Test page, helping with development
 */
function enom_page() {
  #$p = enom_get_tlds(); // Doesn't work with resellertest

  $args = array(
    'sld' => 'example',
    'tld' => 'com',
  );
  #$domains = enom_interface_command('check', $args);
  
  #$p = domains_get_controller('enom')->check(array('randomtimeabcd.com', 'defghijklmn123.net'));
  #$p = domains_get_controller('enom')->getTLDs();
  $p = domains_get_controller('enom')->renew('randomtimeabcd.com');
  #$s = _enom_service();
  #$p = $s->invoke("GetOrderDetail", array('orderid' => '157771821'));
  
  return kpr($p, TRUE);
}

/**
 * Callback for /enom/domains
 * 
 * Page to list registed domain from enom.
 */
function enom_domains_page_callback() {
  $domains = domains_get_controller('enom')->getDomains();
  if ($domains) {
    $header = array();
    foreach ($domains as $domain) {
      if (empty($header)) {
        $header = array_keys($domain);
      }
      $rows[] = $domain;
    }

    return array(
      '#theme'  => 'table',
      '#header' => $header,
      '#rows'   => $rows,
    );
  }
  
  $message = t('No domain found.');
  drupal_set_message($message, 'error');
  
  return '';
}
