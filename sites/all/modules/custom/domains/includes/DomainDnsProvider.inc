<?php

/**
 * Base Dns provider class
 */
abstract class DomainDnsProvider {

  /**
   * Activate a domain to use with this Dns provider
   */
  public abstract function activate($domain);

  /**
   * Check if the domain can be switch to this Dns provider
   */
  public abstract function check($domain);
}
