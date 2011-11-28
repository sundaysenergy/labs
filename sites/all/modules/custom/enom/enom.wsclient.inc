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

  // TODO: make this configurable
  $service->settings['default']['uid'] = "sundaysenergy";
  $service->settings['default']['pw'] = "zola322";

  // wsclient, issue #1302740
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
  $operation['parameter']['command'] = array('type' => 'hidden', 'default value' => 'getdomaininfo');

  $operation['result'] = array('type' => 'enom_result', 'label' => 'Enom result');
  $service->operations['GetDomainInfo'] = $operation;

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