<?php

/**
 * Enom DNS Provider
 */
class EnomDnsProvider extends DomainDnsProvider {

  /**
   * Activate a domain to use with this Dns provider
   */
  public function activate($domain) {
    
  }

  /**
   * Check if the domain can be switch to this Dns provider
   */
  public function check($domain) {
    return TRUE;
  }
}
