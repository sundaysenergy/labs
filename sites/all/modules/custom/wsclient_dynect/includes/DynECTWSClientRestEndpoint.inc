<?php

class DynECTWSClientRestEndpoint extends WSClientRESTEndpoint {
  
  private $_auth_token = NULL;
  
  public function call($operation_name, $arguments) {
    $auth_token = $this->getToken();
    if($operation_name != 'session' && empty($auth_token)) {
      $this->_getToken();
      $auth_token = $this->getToken();

      if (!$auth_token) {
        return FALSE;
      }
    }
    $operation = $this->service->operations[$operation_name];
    $operation_url = isset($operation['url']) ? $operation['url'] : '';
    // Replace argument patterns in the operation URL.
    foreach ($arguments as $key => $value) {
      if (strpos($operation_url, '@' . $key) !== FALSE) {
        $operation_url = str_replace('@' . $key, $value, $operation_url);
        unset($arguments[$key]);
      }
    }
    $client = $this->client();
    $type = isset($operation['type']) ? $operation['type'] : 'GET';
    $data = NULL;
    if (isset($operation['data'])) {
      foreach($operation['data'] as $field) {
        $data[$field] = $arguments[$field];
        unset($arguments[$field]);
      } 
    }
    if (isset($operation['merge_url'])) {
      foreach($operation['merge_url'] as $field) {
        if(!empty($arguments[$field])) {
          $operation_url .= $arguments[$field] . '/';
          unset($arguments[$field]);
        }
      }
    }
    try {
      $response = $client->execute(new HttpClientRequest($this->service->url . $operation_url, array(
        'method' => $type,
        'parameters' => $arguments,
        'data' => $data,
        'headers' => array(
          'Content-Type' => array('application/json'),
          'Auth-Token' => array($auth_token),
        ),
        'options' => array(
          'curlopts' => array(
            CURLOPT_SSL_VERIFYPEER => FALSE,
          ),
        ),
      )));
      return $response;
    }
    catch (HttpClientException $e) {
      watchdog('dynECT', 'Error invoking the REST service %name, operation %operation: %error', array('%name' => $this->service->label, '%operation' => $operation_name, '%error' => $e->getMessage()), WATCHDOG_ERROR);

      // Cause clone error on PHP 5.3.
      //return $e->getResponse();
      return FALSE;
    }
  }
  
  private function _getToken() {
    $result = $this->call('session', array(
      'customer_name' => variable_get('wsclient_dynect_customer_name', ''),
      'user_name' => variable_get('wsclient_dynect_user_name', ''),
      'password' => variable_get('wsclient_dynect_password', ''),
    ));
    if(isset($result['status']) && $result['status'] == 'success') {
      $this->_auth_token = $result['data']['token'];
    }
  }
  
  private function getToken() {
    return $this->_auth_token;
  }
  
}