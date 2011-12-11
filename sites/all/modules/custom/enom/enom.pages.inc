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

  $p = domains_get_provider('enom')->invoke('ModifyNS', array(
    'sld' => 'uniquedomain-1111',
    'tld' => 'com',
    'ns1' => 'ns1.p09.dynect.net',
    'ns2' => 'ns2.p09.dynect.net',
  ));
  
  return kpr($p, TRUE);
}
/**
 * Callback for /enom/domains
 * 
 * Page to list registed domain from enom.
 */
function enom_domains_page_callback() {
  $domains = domains_get_provider('enom')->getDomains();
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