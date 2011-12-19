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
   * Perform mass update for DNS records
   */
  public function updateRecords($domain, $updates) {
    foreach ($updates['new'] as $record) {
      $this->addRecord($domain, $record);
    }

    foreach ($updates['delete'] as $record) {
      $this->deleteRecord($domain, $record);
    }

    foreach ($updates['change'] as $record) {
      $this->updateRecord($domain, $record['#record'], $record);
    }
  }

  /**
   * Add a new record to domain
   */
  public function addRecord($domain, $record) {
    return FALSE;
  }

  /**
   * Update an exist record
   */
  public function updateRecord($domain, $record, $update) {
    return FALSE;
  }

  /**
   * Delete an exist record
   */
  public function deleteRecord($domain, $record) {
    return FALSE;
  }
  
  /**
   * Get supported record types
   */
  public function getRecordTypes() {
    return array(
      'A', 'AAAA', 'CNAME', 'MX', 'NS', 'SRV', 'TXT', 'SPF'
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
