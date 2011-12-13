<?php

/**
 * Domain DNS Provider for DynECT
 */
class DynECTDnsProvider extends DomainDnsProvider {

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

    // Return NS Server.
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

  /**
   * Invoke an operation
   */
  public function invoke($operation, $arguments, $exception=TRUE) {
    $service = wsclient_service_load('dynect');
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
}