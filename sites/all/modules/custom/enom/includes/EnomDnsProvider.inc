<?php

/**
 * Enom DNS Provider
 */
class EnomDnsProvider extends DomainDnsProvider {

  /**
   * Activate a domain to use with this Dns provider
   */
  public function activate($domain, $settings) {
    $entity = domains_domain_load_by_name($domain);

    if ($entity && $entity->controller == 'enom') {
      $domain_pieces = explode(".", $domain);

      domains_get_provider($entity)->invoke('ModifyNS', array(
        'UseDNS' => 'default',
        'tld' => $domain_pieces[1],
        'sld' => $domain_pieces[0],
      ));

      return TRUE;
    }
    else {
      return FALSE;
    }
  }
}
