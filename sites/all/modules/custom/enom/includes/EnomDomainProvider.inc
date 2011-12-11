<?php

/**
 * Enom Domain Controller
 * Implements domain controller interface using enom API
 */
class EnomDomainProvider extends DomainProvider {

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
      "NS1" => "ns1.p09.dynect.net",
      "NS2" => "ns2.p09.dynect.net",
      "NS3" => "ns3.p09.dynect.net",
      "NS4" => "ns4.p09.dynect.net",
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

  public function query($domain, $options=array()) {
    $domain_pieces = explode(".", $domain);
    $result = array();

    $response = $this->invoke("GetDomainInfo", array(
      "tld" => $domain_pieces[1],
      "sld" => $domain_pieces[0],
    ), FALSE);

    if ($response) {
      // TODO: determine which value we need
      // TODO: should we include whois info here too ?
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

  public function getWhois($domain) {
    $domain_pieces = explode(".", $domain);
    $result = array();

    $response = $this->invoke("GetContacts", array(
      "tld" => $domain_pieces[1],
      "sld" => $domain_pieces[0],
    ));

    if ($response) {
      $copies = array(
        'Registrant' => 'registrant',
        'Tech' => 'technical',
        'Admin' => 'admin',
        'AuxBilling' => 'billing',
      );
      foreach ($copies as $name => $target) {
        $info = array(
          'organization' => $response[$name . "OrganizationName"],
          'job_title' => $response[$name . "JobTitle"],
          'first_name' => $response[$name . "FirstName"],
          'last_name' => $response[$name . "LastName"],
          'email' => $response[$name . "EmailAddress"],
          'address1' => $response[$name . "Address"][0],
          'address2' => $response[$name . "Address"][1],
          'phone' => $response[$name . "Phone"],
          'phone_ext' => $response[$name . "PhoneExt"],
          'fax' => $response[$name . "Fax"],
          'country' => $response[$name . "Country"],
          'city' => $response[$name . "City"],
          'state' => $response[$name . "StateProvince"],
          'postal_code' => $response[$name . "PostalCode"]
        );

        $result[$target] = $info;
      }
    }
    else {
      $result = NULL;
    }

    return $result;
  }

  /**
   * Update whois information
   */
  public function updateWhois($domain, $whois) {
    $params = array();
    $copies = array(
      'Registrant' => 'registrant',
      'Tech' => 'technical',
      'Admin' => 'admin',
      'AuxBilling' => 'billing',
    );

    foreach ($copies as $prefix => $type) {
      if (!isset($whois[$type])) continue;

      $params += array(
        $prefix . "OrganizationName" => $whois[$type]['organization'],
        $prefix . "FirstName" => $whois[$type]['first_name'],
        $prefix . "LastName" => $whois[$type]['last_name'],
        $prefix . "JobTitle" => $whois[$type]['job_title'],
        $prefix . "EmailAddress" => $whois[$type]['email'],
        $prefix . "Address1" => $whois[$type]['address1'],
        $prefix . "Address2" => $whois[$type]['address2'],
        $prefix . "City" => $whois[$type]['city'],
        $prefix . "StateProvinceChoice" => "S",
        $prefix . "StateProvince" => $whois[$type]['state'],
        $prefix . "PostalCode" => $whois[$type]['postal_code'],
        $prefix . "Country" => $whois[$type]['country'],
        $prefix . "City" => $whois[$type]['city'],
        $prefix . "Phone" => $whois[$type]['phone'],
        $prefix . "PhoneExt" => $whois[$type]['phone_ext'],
        $prefix . "Fax" => $whois[$type]['fax'],
      );
    }

    if ($params) {
      $domain_pieces = explode(".", $domain);
      $params += array(
        "tld" => $domain_pieces[1],
        "sld" => $domain_pieces[0],
      );

      $response = $this->invoke("Contacts", $params);
      return TRUE;
    }

    return FALSE;
  }

  /**
   * Get nameservers for a domain
   * @param string $domain
   */
  public function getNameservers($domain) {
    $domain_pieces = explode(".", $domain);

    $response = $this->invoke("GetDNS", array(
      "tld" => $domain_pieces[1],
      "sld" => $domain_pieces[0],
    ));

    return $response['DNS'];
  }

  /**
   * Update nameservers for a domain
   * @param string $domain
   * @param array $whois
   */
  public function setNameservers($domain, $ns) {
    $domain_pieces = explode(".", $domain);

    $params = array();
    for ($i = 0; $i < count($ns);$i++) {
      $params['ns' . ($i+1)] = $ns[$i];
    }

    $response = $this->invoke("ModifyNS", $params + array(
      "tld" => $domain_pieces[1],
      "sld" => $domain_pieces[0],
    ));

    return TRUE;
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
  
  public function getDomainName($domain) {
    return $domain['sld'] . '.' . $domain['tld'];
  }
  
  public function getDomainId($domain) {
    return $domain['DomainNameID'];
  }

  /**
   * Invoke an Enom Service operation.
   */
  public function invoke($operation, $params=array(), $exception=TRUE) {
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
        throw new DomainProviderException("Unable to perform request to Enom.", $errors);
      }

      return FALSE;
    }

    return TRUE;
  }
}