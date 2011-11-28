<?php

class EnomWSClientRESTEndpoint extends WSClientRESTEndpoint {
  public function call($operation_name, $arguments) {
    // fill with default value
    foreach (array('uid', 'pw') as $param) {
      if (empty($arguments[$param])) {
        if (isset($this->service->settings['default'], $this->service->settings['default'][$param])) {
          $arguments[$param] = $this->service->settings['default'][$param];
        }
      }
    }
    return parent::call($operation_name, $arguments);
  }
}
