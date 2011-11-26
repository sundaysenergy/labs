<?php

/**
 * Domain controller api interface
 */
interface DomainControllerInterface {

  /**
   * Check a list of domains for its avaiability
   * @param array $domains
   * @return array
   */
  public function check($domains);

  /**
   * Get domain info
   * @param string $domain
   * @return array
   */
  public function getInfo($domain);

  /**
   * Renew a domain
   * @param string $domain domain name
   * @param array $options optional options
   */
  public function renew($domain, $options=array());

  /**
   * Purchase a domain
   * @param string $domain domain name
   * @param array $options optional options
   */
  public function purchase($domain, $options=array());

  /**
   * Return supported TLDs
   * @return array
   */
  public function getTLDs();
}


/**
 * Domain controller exception class.
 */
class DomainControllerException extends Exception {

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

  public function getAdditionalErrors() {
    return $this->errors;
  }
}
