<?php

/**
 * Enom Rest client
 * 
 * Implement additional processing at service-level.
 */
class EnomWSClientRESTEndpoint extends WSClientRESTEndpoint {
  public function call($operation_name, $arguments) {
    // Fill with default value.
    if (empty($arguments['uid'])) {
      $arguments['uid'] = variable_get('enom_login_uid', '');
      $arguments['pw'] = variable_get('enom_login_pw', '');
    }

    // Empty arguments must not be pass.
    if ($arguments['command'] == 'ModifyNS') {
      foreach ($arguments as $name => $value) {
        if (substr($name, 0, 2) == 'ns' && NULL === $value) {
          unset($arguments[$name]);
        }
      }

      if ($arguments['usedns'] === NULL) {
        unset($arguments['usedns']);
      }
    }

    return parent::call($operation_name, $arguments);
  }
}