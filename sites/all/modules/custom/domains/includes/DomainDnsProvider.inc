<?php

/**
 * Base Dns provider class
 */
abstract class DomainDnsProvider {

  /**
   * Activate a domain to use with this Dns provider
   * @return array Nameservers
   */
  public abstract function activate($domain);

  /**
   * Check if the domain can be switch to this Dns provider
   */
  public function check($domain) {
    // Most of the time, we just need to change DNS Nameservers.
    return TRUE;
  }
}
