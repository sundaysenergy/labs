<?php

/**
 * @file
 * 
 * Domain entity class
 */

/**
 * Domain entity
 */
class DomainEntity extends Entity {

  /**
   * Domain name
   * @var string
   */
  public $name;

  /**
   * Controller key
   * @var string
   */
  public $controller;

  /**
   * Owner of this domain
   * @var int
   */
  public $uid;

  /**
   * Expiration time in GMT
   * @var int
   */
  public $expiration;

  /**
   * Get domain controller object
   * @return DomainProviderInterface
   */
  public function getController() {
    return domains_get_provider($this->controller);
  }
}
