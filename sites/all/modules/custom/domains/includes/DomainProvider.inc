<?php

/**
 * Domain provder base class
 */
abstract class DomainProvider implements DomainProviderInterface {
  protected $info;

  private $uiController;

  /**
   * Constructor
   */
  public function __construct($provider_info) {
    $this->info = $provider_info;
  }

  public function getUiController() {
    if (isset($this->info['ui class'])) {
      if (!$this->uiController) {
        $class = $this->info['ui class'];
        $this->uiController = new $class($this);
      }

      return $this->uiController;
    }
    else {
      return NULL;
    }
  }

  public function getInfo() {
    return $this->info;
  }
}

/**
 * Domain controller exception class.
 */
class DomainProviderException extends Exception {

  /**
   * Additional errors info
   * @var array
   */
  protected $errors = array();

  /**
   * Constructor
   * @param $message General error message
   * @param $errors Additional error array
   */
  public function __construct($message, $errors=array()) {
    parent::__construct($message);
    $this->errors = $errors;
  }

  /**
   * Get additional error information
   */
  public function getAdditionalErrors() {
    return $this->errors;
  }
}