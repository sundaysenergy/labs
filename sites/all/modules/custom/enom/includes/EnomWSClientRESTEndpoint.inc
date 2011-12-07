<?php

// TODO: We may remove this class later.
class EnomWSClientRESTEndpoint extends WSClientRESTEndpoint {
  public function call($operation_name, $arguments) {
    return parent::call($operation_name, $arguments);
  }
}
