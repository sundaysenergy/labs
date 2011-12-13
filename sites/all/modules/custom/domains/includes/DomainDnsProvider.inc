<?php

/**
 * Base Dns provider class
 */
abstract class DomainDnsProvider {

  /**
   * Activate a domain to use with this Dns provider
   * @return array Nameservers
   */
  public abstract function activate($domain, $settings);

  /**
   * Check if the domain can be switch to this Dns provider
   */
  public function check($domain) {
    // Most of the time, we just need to change DNS Nameservers.
    return TRUE;
  }

  /**
   * Get all records for domains
   */
  public function getRecords($domain) {
    return FALSE;
  }

  /**
   * Get supported record types
   */
  public function getRecordTypes() {
    return array(
      'A', 'AAAA', 'CNAME', 'MX', 'SOA', 'SPF', 'SRV', 'TXT'
    );
  }
}

/**
 * Custom DNS provider
 */
class CustomDnsProvider extends DomainDnsProvider {
  public function activate($domain, $settings) {
    return array_filter($settings['ns']);
  }
}
