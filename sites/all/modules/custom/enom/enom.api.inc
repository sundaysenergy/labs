<?php

/**
 * @file
 * 
 * Enom API
 * Group of enom api functions
 * 
 * @link https://www.enom.com/resellers/APICommandCatalog.pdf
 */

/**
 * @param $response array returned from enom service
 * @param $set_message - Print errors to screen
 * @return bool - True if it finds errors
 */
function enom_error_check($response) {
  $errors = array();

  if (!isset($response['ErrCount'])) {
    $errors[] = t("Did not get a valid Server response.  No 'ErrCount' found. Please try again later.");
  }
  else if ($response['ErrCount'] && count($response['Err'])) {
    $errors = $response['Err'];
  }
  else if ($response['ErrCount'] && isset($response['Err1'])) {
    for ($i = 1; $i <= $res['ErrCount']; $i++) {
      $errors[] = $response['Err'. $i];
    }
    if (isset($response['ResponseString1'])) {
      $errors[] = $response['ResponseString1'];
    }
    if (isset($response['ResponseNumber1'])) {
      $errors[] = $response['ResponseNumber1'];
    }
  }

  if (!empty($errors)) {
    $msg = t('An error occured. !errors');
    $tokens = array('!errors' => theme('item_list', array('items' => $errors)));

    watchdog('enom', $msg, $tokens, WATCHDOG_ERROR);
  }
  
  return $errors;
}

/**
 * Build up a basic service
 * @return WSClientServiceDescription
 */
function _enom_service() {
  $service = wsclient_service_load('enom');
  $service->settings['formatter'] = new HttpClientINIFormatter(); # new HttpClientXMLFormatter();
  if ($proxy = variable_get('enom_proxy', FALSE)) {
    $service->settings['curl options'] = array(
      CURLOPT_PROXYPORT => variable_get('enom_proxy_port', 8888),
      CURLOPT_HTTPPROXYTUNNEL => TRUE,
      CURLOPT_PROXYTYPE => variable_get('enom_proxy_type', CURLPROXY_HTTP),
      CURLOPT_PROXY => $proxy,
    );
  }
  return $service;
}
