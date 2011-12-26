<?php

/**
 * Domain DNS Provider for DynECT
 */
class DynECTDnsProvider extends DomainDnsProvider {

  /**
   * WSclient service
   */
  private $service;

  public function activate($domain, $settings) {
    $response = $this->invoke('get_zone', array(
      'zone' => $domain,
    ), FALSE);

    if (!$response) {
      $response = $this->invoke('create_zone', array(
        'zone_name' => $domain,
        'rname' => "admin@$domain",
        'zone' => $domain,
        'ttl' => 3600,
      ));

      $this->invoke('publish_zone', array(
        'zone' => $domain,
      ));
    }

    // Return NS Records for this domain.
    $ns = array();
    $response = $this->invoke('get_all_record', array('zone' => $domain));
    foreach ($response['data'] as $record_uri) {
      $record = _wsclient_dynect_record_query($record_uri);

      if ($record['record_type'] == 'NSRecord') {
        $record_detail = $this->invoke('get_record', $record);
        $host = substr($record_detail['data']['rdata']['nsdname'], 0, -1);
        $ip = _wsclient_dynect_resolve_ip($host);
        $ns[$host] = $ip;
      }
    }

    return $ns;
  }

  public function getRecords($domain) {
    $response = $this->invoke('get_all_record', array('zone' => $domain));

    $result = array();
    foreach ($response['data'] as $record_uri) {
      $record = _wsclient_dynect_get_record($this->getService(), $record_uri);
      if ($record) {
        $result[] = $this->normalizeRecord($record);
      }
    }

    return $result;
  }

  /**
   * Perform mass update for DNS records
   */
  public function updateRecords($domain, $updates) {
    foreach ($updates['new'] as $record) {
      $this->_addRecord($domain, $record);
    }

    foreach ($updates['delete'] as $record) {
      $this->_deleteRecord($domain, $record);
    }

    foreach ($updates['change'] as $record) {
      $this->_updateRecord($domain, $record['#record'], $record);
    }

    $this->publishChange($domain);
    return TRUE;
  }

  public function addRecord($domain, $record) {
    $this->_addRecord($domain, $record);
    $this->publishChange($domain);
  }

  protected function _addRecord($domain, $record) {
    $dynect_record = $this->denormalizeRecord($domain, $record);
    if (!$dynect_record) {
      throw new DomainException("Unsupported record type");
    }

    $this->invoke('create_record', $dynect_record);
  }

  public function deleteRecord($domain, $record) {
    $result = $this->_deleteRecord($domain, $record);
   	$this->publishChange($domain);
  }

  protected function _deleteRecord($domain, $record) {
    if (isset($record['#record'])) {
      $record = $record['#record'];
    }

    $this->invoke("delete_record", array(
      'record_type' => $record['record_type'],
      'query' => "{$record['zone']}/{$record['fqdn']}/{$record['record_id']}",
    ));
  }

  public function updateRecord($domain, $record, $update) {
    $this->_updateRecord($domain, $record, $update);
    $this->publishChange($domain);
  }
  
  protected function _updateRecord($domain, $record, $update) {
    $dynect_record = $this->denormalizeRecord($domain, $update);
    if (!$dynect_record) {
      throw new DomainException("Unsupported record type");
    }
    if (empty($record['record_id'])) {
      throw new DomainException("Record ID is missing");
    }

    if ($dynect_record['fqdn'] == $record['fqdn'] && $dynect_record['record_type'] == $record['record_type']) {
      $dynect_record['record_id'] = $record['record_id'];
      $this->invoke('update_record', $dynect_record);
    }
    else {
      $this->_deleteRecord($domain, $record);
      $this->_addRecord($domain, $update);
    }
  }

  public function publishChange($domain) {
    $this->invoke('publish_zone', array(
      'zone' => $domain,
    ));
  }

  /**
   * Invoke an operation
   */
  public function invoke($operation, $arguments, $exception=TRUE) {
    $service = $this->getService();
    $response = $service->invoke($operation, $arguments);
    if($response['status'] == 'success') {
      return $response;
    }
    else {
      $error = array();
      if ($response) {
        foreach($response['msgs'] as $message){
          $message = (object) $message;
          $error[] = ($message->ERR_CD != '' ? '('.$message->ERR_CD.') ' : '').$message->SOURCE." - ".$message->INFO;
        }
      }

      if ($exception) {
        throw new DomainProviderException(t('Unable to perform service operations.'), $error);
      }
    }
  }

  /**
   * Make record data return from DynECT compatible with Domains
   */
  public function normalizeRecord($record) {
    $record['readonly'] = FALSE;
    $record['priority'] = '';

    switch ($record['type']) {
      case 'AAAA':
      case 'A':
        $record['value'] = $record['rdata']['address'];
        break;
      case 'CNAME':
        $record['value'] = $record['rdata']['cname'];
        break;
      case 'PTR':
        $record['value'] = $record['rdata']['ptrdname'];
        break;
      case 'MX':
        $record['value'] = $record['rdata']['exchange'];
        $record['priority'] = $record['rdata']['preference'];
        break;
      case 'TXT':
        $record['value'] = $record['rdata']['txtdata'];
        break;

      case 'NS':
        $record['value'] = $record['rdata']['nsdname'];
        $record['display'] = $record['value'];
        $record['readonly'] = TRUE;
        $record['name'] = $record['fqdn'];
        break;

      default:
        $list = array();
        foreach($record['rdata'] as $key => $value) {
          $list[] = $key . ': ' . $value;
        }

        $record['readonly'] = TRUE;
        $record['display'] = theme('item_list', array('items' => $list));

        if ($record['type'] == 'SOA') {
          $record['name'] = $record['fqdn'];
        }

        break;
    }

    // We don't want to repeat the full domain name again and again.
    if (empty($record['name'])) {
      $record['name'] = str_replace($record['zone'], "", $record['fqdn']);
      if (substr($record['name'], -1) == '.') {
        $record['name'] = substr($record['name'], 0, -1);
      }
    }

    return $record;
  }

  /**
   * Convert normal record array to dynect record data
   */
  public function denormalizeRecord($domain, $record) {
    if ($record['name'] != '') {
      $fqdn = "{$record['name']}.$domain";
    }
    else {
      $fqdn = $domain;
    }

    $new_record = array(
      'ttl' => 0,
      'zone' => $domain,
      'fqdn' => $fqdn,
    );

    // Only support a few record so far.
    switch ($record['type']) {
      case 'PTR':
        $new_record['record_type'] = 'PTRRecord';
        $new_record['rdata']['ptrdname'] = $record['value'];
        break;

      case 'TXT':
        $new_record['record_type'] = 'TXTRecord';
        $new_record['rdata']['txtdata'] = $record['value'];
        break;

      case 'MX':
        $new_record['record_type'] = 'MXRecord';
        $new_record['rdata']['exchange'] = $record['value'];
        $new_record['rdata']['preference'] = $record['priority'];
        break;

      case 'CNAME':
        $new_record['record_type'] = 'CNAMERecord';
        $new_record['rdata']['cname'] = $record['value'];
        break;

      case 'AAAA':
        $new_record['record_type'] = 'AAAARecord';
        $new_record['rdata']['address'] = $record['value'];
        break;

      case 'A':
        $new_record['record_type'] = 'ARecord';
        $new_record['rdata']['address'] = $record['value'];
        break;

      default:
        $new_record = null;
    }

    return $new_record;
  }

  public function getService() {
    if (!$this->service) {
      $this->service = wsclient_service_load('dynect');
    }

    return $this->service;
  }

  /**
   * Get supported record types
   */
  public function getRecordTypes() {
    return array(
        'A', 'AAAA', 'CNAME', 'MX', 'TXT', 'SPF'
    );
  }
}