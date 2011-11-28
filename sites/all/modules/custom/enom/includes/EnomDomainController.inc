<?php

/**
 * Enom Domain Controller
 * Implements domain controller interface using enom API
 */
class EnomDomainController implements DomainControllerInterface {
  
  public function check($domains) {
    $results = array();
    $list = implode(",", $domains);

    $response = $this->invoke('checkMulti', array('domainlist' => $list));
    $success_code = 210;

    // always return many results
    if (isset($response['RRPCode']) && is_array($response['RRPCode']) && is_array($response['Domain'])) {
      foreach ($response['RRPCode'] as $key => $code) {
        $results[$response['Domain'][$key]] = ($success_code == $code);
      }
    }
    else {
      $results = FALSE;
    }

    return $results;
  }
  
  public function purchase($domain, $options = array()) {
    $domain_pieces = explode(".", $domain);
    $result = $this->invoke("Purchase", array(
      "tld" => $domain_pieces[1],
      "sld" => $domain_pieces[0],
      "usedns" => "default",
    ));

    // TODO: enom may return RPRCode = 1300 and IsRealTimeTLD = FALSE
    // in this case, we will need to call GetOrderDetail daily to check
    // the registration prgress.

    return ($result['RRPCode'] == 200);
  }

  public function renew($domain, $options=array()) {
    $options = $options + array('years' => 1);
    $domain_pieces = explode(".", $domain);

    $response = $this->invoke("Extend", array(
      "tld" => $domain_pieces[1],
      "sld" => $domain_pieces[0],
      "numyears" => $options['years'],
      "overrideorder" => 1,
    ));

    return $response['RRPCode'] == 200;
  }

  public function getInfo($domain) {
    $domain_pieces = explode(".", $domain);
    $result = array();

    $response = $this->invoke("GetDomainInfo", array(
      "tld" => $domain_pieces[1],
      "sld" => $domain_pieces[0],
    ), FALSE);

    if ($response) {
      // TODO: determine which value we need
      $result = array(
        'name' => $response['domainname'],
        'expiration' => strtotime($response['expiration']),
      );
    }
    else {
      $result = NULL;
    }

    return $result;
  }

  public function getTLDs() {
    $response = $this->invoke('GetTldList');
    $tlds = array();

    foreach ($response['tld'] as $tld) {
      if ($tld != "") {
        $tlds[] = $tld;
      }
    }

    return $tlds;
  }

  public function getDomains($start = 1) {
    $response = $this->invoke('GetDomains', array('start' => $start));
    if (!$response['ErrCount']) {
      $domains = $this->parseDomains($response);
      if (!empty($response['NextRecords'])) {
        $domains = array_merge($domains, $this->getDomains($response['NextRecords'])); 
      }
      return $domains;
    }
    return FALSE;
  }

  private function parseDomains($reponse) {
    for ($i = 0; $i < $reponse['count']; ++$i) {
      $domain = array();
      foreach (array('DomainNameID', 'sld', 'tld', 'ns-status', 'expiration-date', 'auto-renew', 'wppsstatus', 'RRProcessor') as $k) {
        $domain[$k] = $reponse[$k][$i];
      }
      $domains[] = $domain;
    }
    return $domains;
  }

  /**
   * Invoke an Enom Service operation.
   */
  protected function invoke($operation, $params = array(), $exception = TRUE) {
    $service = _enom_service();
    $result = $service->invoke($operation, $params);
    if ($this->validateResponse($result, $exception)) {
      return $result;
    }
    return FALSE;
  }

  /**
   * Validate response from API, Throw exception if necessary.
   */
  protected function validateResponse($response, $exception=TRUE) {
    if ($errors = enom_error_check($response)) {
      if ($exception) {
        // use a friendly message
        throw new DomainControllerException("Unable to perform request to Enom.", $errors);
      }
      return FALSE;
    }
    return TRUE;
  }
}
