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
   * DNS controller
   * @var string
   */
  public $dns_provider;

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
}
