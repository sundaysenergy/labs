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
}