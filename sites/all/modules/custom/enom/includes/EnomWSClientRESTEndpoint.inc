<?php

// TODO: We may remove this class later.
class EnomWSClientRESTEndpoint extends WSClientRESTEndpoint {
  public function call($operation_name, $arguments) {
    // Fill with default value.
    if (empty($arguments['uid'])) {
      $arguments['uid'] = variable_get('enom_login_uid', '');
      $arguments['pw'] = variable_get('enom_login_pw', '');
    }

    return parent::call($operation_name, $arguments);
  }
}