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
        throw new DomainProviderException(t('Unable to perform service operations.', $error));
      }
    }
  }

  /**
   * Make record data return from DynECT compatible with Domains
   */
  public function normalizeRecord($record) {
    $record['readonly'] = FALSE;

    switch ($record['type']) {
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

  public function getService() {
    if (!$this->service) {
      $this->service = wsclient_service_load('dynect');
    }

    return $this->service;
  }
}
