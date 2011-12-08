<?php

/**
 * @file
 * 
 * Enom WSClient description
 */

/**
 * Implements hook_default_wsclient_service().
 * This defines all the enom calls
 */
function enom_default_wsclient_service() {
  // Enom service (RESTish)
  $service = new WSClientServiceDescription();
  $service->name = 'enom';
  $service->label = 'Enom Service APIs';
  $service->url = 'http://resellertest.enom.com/interface.asp';
  $service->type = 'enom_rest';

  $service->global_parameters['uid'] = array(
    'default value' => '',
  );

  $service->global_parameters['pw'] = array(
    'default value' => '',
  );

  // Add info about the REST service, which the user has to provide via the UI.
  $service->datatypes = array(
    'enom_result' => array(
      'label' => 'Enom result',
      'property info' => array(
        // array key
        'RRPCode' => array(
          'type' => 'integer',
          //'type' => 'struct', // array
          'label' => 'Result code. 210 = available; 211 = unavailable',
        ),
        'RRPText' => array(
          'type' => 'text',
          //'type' => 'struct', // array
          'label' => 'Result text.',
        ),
      ),
    ),
  );

  // Operation: Check Domain
  $operation = array();
  $operation['label'] = 'Check domain';
  $operation['parameter']['sld'] = array(
    'type' => 'text',
    'label' => 'Second Level Domain',
    'description' => 'The name placed to the left of the TLD separated by a dot.
       In "example.com", "example" is the SLD and "com" is the TLD.',
  );
  $operation['parameter']['tld'] = array(
    'type' => 'text',
    'label' => 'Top Level Domain',
    'description' => 'The name placed to the right of the SLD.
        In "example.com", "com" is the TLD. * returns 11 commonly used TLDs.
        *1 returns 7, *2 returns 6, @ returns com, net, org',
  );
  $operation['parameter']['command'] = array('type' => 'hidden', 'default value' => 'check');

  $operation['result'] = array('type' => 'enom_result', 'label' => 'Enom result');
  $service->operations['check'] = $operation;

  // Operation: CheckMulti
  // this function is just Check() but different parameters
  $operation = array();
  $operation['label'] = 'Check multi domains';
  $operation['parameter']['domainlist'] = array(
    'type' => 'text',
    'label' => 'Domain List',
    'description' => 'A comma- or newline-delimited list of domains to check, up to 30 names.',
  );
  $operation['parameter']['command'] = array('type' => 'hidden', 'default value' => 'check');

  $operation['result'] = array('type' => 'enom_result', 'label' => 'Enom result');
  $service->operations['checkMulti'] = $operation;

  // Operation: GetTLDList
  $operation = array();
  $operation['label'] = 'Get TLD List';
  $operation['parameter']['command'] = array('type' => 'hidden', 'default value' => 'gettldlist');

  $operation['result'] = array('type' => 'enom_result', 'label' => 'Enom result');
  $service->operations['GetTldList'] = $operation;

  // Operation: GetOrderList
  $operation = array();
  $operation['label'] = 'Get Order List';
  $operation['parameter']['start'] = array('type' => 'text', 'default value' => '');
  $operation['parameter']['command'] = array('type' => 'hidden', 'default value' => 'getorderlist');

  $operation['result'] = array('type' => 'enom_result', 'label' => 'Enom result');
  $service->operations['GetOrderList'] = $operation;

  // Operation: GetOrderList
  $operation = array();
  $operation['label'] = 'Get Order Detail';
  $operation['parameter']['orderid'] = array('type' => 'text', 'default value' => '');
  $operation['parameter']['command'] = array('type' => 'hidden', 'default value' => 'getorderdetail');

  $operation['result'] = array('type' => 'enom_result', 'label' => 'Enom result');
  $service->operations['GetOrderDetail'] = $operation;

  // Operation: GetDomains
  $operation = array();
  $operation['label'] = 'Get purchased domains';
  $operation['parameter']['tab'] = array('type' => 'hidden', 'default value' => 'IOwn');
  $operation['parameter']['display'] = array('type' => 'hidden', 'default value' => 25);
  $operation['parameter']['start'] = array('type' => 'text', 'default value' => 1);
  $operation['parameter']['command'] = array('type' => 'hidden', 'default value' => 'GetDomains');
  $operation['result'] = array('type' => 'enom_result', 'label' => 'Enom result');
  $service->operations['GetDomains'] = $operation;
  
  // Operation: GetDomainInfo
  $operation = array();
  $operation['label'] = 'Get Domain Info';
  $operation['parameter']['sld'] = array('type' => 'text', 'label' => 'Second Level Domain');
  $operation['parameter']['tld'] = array('type' => 'text', 'label' => 'Top Level Domain');
  $operation['parameter']['command'] = array('type' => 'hidden', 'default value' => 'GetDomainInfo');

  $operation['result'] = array('type' => 'enom_result', 'label' => 'Enom result');
  $service->operations['GetDomainInfo'] = $operation;

  // Operation: GetDNS
  $operation = array();
  $operation['label'] = 'Get DNS';
  $operation['parameter']['sld'] = array('type' => 'text', 'label' => 'Second Level Domain');
  $operation['parameter']['tld'] = array('type' => 'text', 'label' => 'Top Level Domain');
  $operation['parameter']['command'] = array('type' => 'hidden', 'default value' => 'GetDNS');

  $operation['result'] = array('type' => 'enom_result', 'label' => 'Enom result');
  $service->operations['GetDNS'] = $operation;

  // Operation: ModifyNS
  $operation = array();
  $operation['label'] = 'Modify NS';
  $operation['parameter']['sld'] = array('type' => 'text', 'label' => 'Second Level Domain');
  $operation['parameter']['tld'] = array('type' => 'text', 'label' => 'Top Level Domain');

  for ($i = 1;$i <= 12;$i++) {
    $operation['parameter']['ns' . $i] = array('type' => 'text', 'label' => 'Nameserver ' . $i, 'optional' => TRUE);
  }

  $operation['parameter']['command'] = array('type' => 'hidden', 'default value' => 'ModifyNS');

  $operation['result'] = array('type' => 'enom_result', 'label' => 'Enom result');
  $service->operations['ModifyNS'] = $operation;

  // Operation: GetContacts
  $operation = array();
  $operation['label'] = 'Get Contacts';
  $operation['parameter']['sld'] = array('type' => 'text', 'label' => 'Second Level Domain');
  $operation['parameter']['tld'] = array('type' => 'text', 'label' => 'Top Level Domain');
  $operation['parameter']['command'] = array('type' => 'hidden', 'default value' => 'GetContacts');

  $operation['result'] = array('type' => 'enom_result', 'label' => 'Enom result');
  $service->operations['GetContacts'] = $operation;

  // Operation: Contacts
  $operation = array();
  $operation['label'] = 'Contacts';
  $operation['parameter']['sld'] = array('type' => 'text', 'label' => 'Second Level Domain');
  $operation['parameter']['tld'] = array('type' => 'text', 'label' => 'Top Level Domain');
  $types = array('Registrant', 'Tech', 'Admin', 'AuxBilling');

  foreach ($types as $name) {
    $operation['parameter'] += array(
      $name . "OrganizationName" => array('type' => 'text', 'label' => "$name Organization name"),
      $name . "JobTitle" => array('type' => 'text', 'label' => "$name Job title"),
      $name . "FirstName" => array('type' => 'text', 'label' => "$name First name"),
      $name . "LastName" => array('type' => 'text', 'label' => "$name Last name"),
      $name . "EmailAddress" => array('type' => 'text', 'label' => "$name Email Address"),
      $name . "Address1" => array('type' => 'text', 'label' => "$name Address 1"),
      $name . "Address2" => array('type' => 'text', 'label' => "$name Address 2"),
      $name . "Phone" => array('type' => 'text', 'label' => "$name Phone"),
      $name . "Fax" => array('type' => 'text', 'label' => "$name Fax"),
      $name . "PhoneExt" => array('type' => 'text', 'label' => "$name Phone extension"),
      $name . "Country" => array('type' => 'text', 'label' => "$name Country"),
      $name . "City" => array('type' => 'text', 'label' => "$name City"),
      $name . "StateProvince" => array('type' => 'text', 'label' => "$name State / Provine"),
      $name . "StateProvinceChoice" => array('type' => 'text', 'label' => "$name State / Province Choice"),
      $name . "PostalCode" => array('type' => 'text', 'label' => "$name Postal code"),
    );
  }
  $operation['parameter']['command'] = array('type' => 'hidden', 'default value' => 'Contacts');

  $operation['result'] = array('type' => 'enom_result', 'label' => 'Enom result');
  $service->operations['Contacts'] = $operation;

  // Operation: Purchase
  $operation = array();
  $operation['label'] = 'Purchase';
  $operation['parameter']['sld'] = array('type' => 'text', 'label' => 'Second Level Domain');
  $operation['parameter']['tld'] = array('type' => 'text', 'label' => 'Top Level Domain');
  $operation['parameter']['usedns'] = array('type' => 'text', 'default value' => '');
  $operation['parameter']['command'] = array('type' => 'hidden', 'default value' => 'purchase');

  $operation['result'] = array('type' => 'enom_result', 'label' => 'Enom result');
  $service->operations['Purchase'] = $operation;

  // Operation: Extend
  $operation = array();
  $operation['label'] = 'Extend';
  $operation['parameter']['sld'] = array('type' => 'text', 'label' => 'Second Level Domain');
  $operation['parameter']['tld'] = array('type' => 'text', 'label' => 'Top Level Domain');
  $operation['parameter']['numyears'] = array('type' => 'integer', 'default value' => '1');
  $operation['parameter']['overrideorder'] = array('type' => 'integer', 'default value' => '1');
  $operation['parameter']['command'] = array('type' => 'hidden', 'default value' => 'extend');

  $operation['result'] = array('type' => 'enom_result', 'label' => 'Enom result');
  $service->operations['Extend'] = $operation;

  // Operation: DeleteRegistration
  $operation = array();
  $operation['label'] = 'Delete Registration';
  $operation['parameter']['sld'] = array('type' => 'text', 'label' => 'Second Level Domain');
  $operation['parameter']['tld'] = array('type' => 'text', 'label' => 'Top Level Domain');
  $operation['parameter']['enduserip'] = array('type' => 'text', 'label' => 'End User IP');
  $operation['parameter']['command'] = array('type' => 'hidden', 'default value' => 'DeleteRegistration');

  $operation['result'] = array('type' => 'enom_result', 'label' => 'Enom result');
  $service->operations['DeleteRegistration'] = $operation;

  // Operation: GetAllAccountInfo
  $operation = array();
  $operation['label'] = 'Get All Account Info';
  $operation['parameter']['command'] = array('type' => 'hidden', 'default value' => 'GetAllAccountInfo');

  $operation['result'] = array('type' => 'enom_result', 'label' => 'Enom result');
  $service->operations['GetAllAccountInfo'] = $operation;

  // Operation: UpdateAccountInfo
  $operation = array();
  $operation['label'] = 'Update Account Info';
  $operation['parameter']['command'] = array('type' => 'hidden', 'default value' => 'UpdateAccountInfo');
  $operation['parameter']['Account'] = array('type' => 'text', 'label' => 'Account');
  $operation['parameter']['NewUID'] = array('type' => 'text', 'label' => 'New account UID');
  $operation['parameter']['NewPW'] = array('type' => 'text', 'label' => 'New password');
  $operation['parameter']['ConfirmNewPW'] = array('type' => 'text', 'label' => 'Confirm new password');
  $operation['parameter']['RegistrantEmailAddress'] = array('type' => 'text', 'label' => 'Email');
  $operation['parameter']['AuthQuestionType'] = array('type' => 'text', 'label' => 'Auth Question Type');
  $operation['parameter']['AuthQuestionAnswer'] = array('type' => 'text', 'label' => 'Auth Question Answer');
  $operation['parameter']['RegistrantOrganizationName'] = array('type' => 'text', 'label' => 'Organization name');
  $operation['parameter']['RegistrantFirstName'] = array('type' => 'text', 'label' => 'First name');
  $operation['parameter']['RegistrantLastName'] = array('type' => 'text', 'label' => 'Last name');
  $operation['parameter']['RegistrantJobTitle'] = array('type' => 'text', 'label' => 'Job title');
  $operation['parameter']['RegistrantAddress1'] = array('type' => 'text', 'label' => 'Address1');
  $operation['parameter']['RegistrantAddress2'] = array('type' => 'text', 'label' => 'RegistrantAddress2');
  $operation['parameter']['RegistrantCity'] = array('type' => 'text', 'label' => 'City');
  $operation['parameter']['RegistrantStateProvinceChoice'] = array('type' => 'text', 'label' => 'State or Provine choice', 'description' => t('S for state, P for provine'));
  $operation['parameter']['RegistrantStateProvince'] = array('type' => 'text', 'label' => 'State / Provine');
  $operation['parameter']['RegistrantCountry'] = array('type' => 'text', 'label' => 'Country');
  $operation['parameter']['RegistrantPostalCode'] = array('type' => 'text', 'label' => 'Postal code');
  $operation['parameter']['RegistrantPhone'] = array('type' => 'text', 'label' => 'Phone');
  $operation['parameter']['RegistrantFax'] = array('type' => 'text', 'label' => 'Fax');

  $operation['result'] = array('type' => 'enom_result', 'label' => 'Enom result');
  $service->operations['UpdateAccountInfo'] = $operation;

  $services[$service->name] = $service;
  return $services;
}


/**
 * Implements hook_wsclient_endpoint_types().
 */
function enom_wsclient_endpoint_types() {
  return array(
    'enom_rest' => array(
      'label' => t('Enom REST'),
      'class' => 'EnomWSClientRESTEndpoint',
    ),
  );
}