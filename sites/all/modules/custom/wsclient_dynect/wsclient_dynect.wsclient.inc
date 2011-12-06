<?php

/**
 * @file
 * 
 * dynECT WSClient description
 */


/**
 * Implements hook_wsclient_endpoint_types().
 */
function wsclient_dynect_wsclient_endpoint_types() {
  return array(
    'dynect_rest' => array(
      'label' => t('dynECT REST'),
      'class' => 'DynECTWSClientRestEndpoint',
    ),
  );
}

/**
 * Implements hook_default_wsclient_service().
 */
function wsclient_dynect_default_wsclient_service() {
  // dynECT api service (REST)
  $service = new WSClientServiceDescription();
  $service->name = 'dynect';
  $service->label = 'dynECT API';
  $service->url = 'https://api2.dynect.net/REST';
  $service->type = 'dynect_rest';
  
  // Add info about the REST service, which the user has to provide via the UI.
  $service->datatypes = array(
    'dynect_result' => array(
      'label' => 'dynECT Result',
      'property info' => array(
        'status' => array(
          'type' => 'text',
          'label' => 'Response status',
        ),
        'data' => array(
          'type' => 'text',
          'label' => 'Reponse data',
        ),
        'job_id' => array(
          'type' => 'integer',
          'label' => 'Job ID'
        ),
        'msgs' => array(
          'type' => 'text',
          'label' => 'Reponse message',
        ),
      ),
    ),
  );
  
  // Session
  $session_operations['label'] = 'Session';
  $session_operations['url'] = '/Session/';
  $session_operations['type'] = 'POST';
  $session_operations['parameter']['customer_name'] = array('type' => 'text', 'label' => 'Customer Name');
  $session_operations['parameter']['user_name'] = array('type' => 'text', 'label' => 'User name');
  $session_operations['parameter']['password'] = array('type' => 'text', 'label' => 'Password');
  // POST field
  $session_operations['data'] = array('customer_name', 'user_name', 'password');
  $session_operations['result'] = array('type' => 'dynect_result', 'label' => 'Session Result');
  $service->operations['session'] = $session_operations;
  
  // Get all zone
  $get_zones_operations = array(
    'label' => 'Get Zones',
    'url' => '/Zone/',
    'type' => 'GET',
    'result' => array('type' => 'dynect_result', 'label' => 'Session Result'),
  );
  $service->operations['get_all_zone'] = $get_zones_operations;
  
  // Get detail zone
  $get_zones_operations = array(
    'label' => 'Get Zones',
    'url' => '/Zone/',
    'type' => 'GET',
    'parameter' => array(
      'zone' => array('type' => 'text', 'label' => 'Zone'),
    ),
    'merge_url' => array('zone'),
    'result' => array('type' => 'dynect_result', 'label' => 'Session Result'),
  );
  $service->operations['get_zone'] = $get_zones_operations;
  
  $services[$service->name] = $service;
  
  return $services;
}