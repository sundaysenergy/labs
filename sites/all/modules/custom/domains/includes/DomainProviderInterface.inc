<?php

/**
 * Domain controller api interface
 */
interface DomainProviderInterface {

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
  public function query($domain, $options=array());

  /**
   * Get whois information for a domain
   * @param string $domain
   */
  public function getWhois($domain);

  /**
   * Update whois information for a domain
   * @param string $domain
   * @param array $whois
   */
  public function updateWhois($domain, $whois);

  /**
   * Get nameservers for a domain
   * @param string $domain
   */
  public function getNameservers($domain);

  /**
   * Update nameservers for a domain
   * @param string $domain
   * @param array $whois
   */
  public function setNameservers($domain, $ns);

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

  /**
   * Get UI controller
   * @return DomainProviderUI
   */
  public function getUiController();

  /**
   * Get provider info
   * @return array
   */
  public function getInfo();

  /**
   * Get domain name
   * @return string Domain name and it ltd
   */
  public function getDomainName($domain);

  /**
   * Get domain id
   * @return string An domain identify on provider
   */
  public function getDomainId($domain);
}