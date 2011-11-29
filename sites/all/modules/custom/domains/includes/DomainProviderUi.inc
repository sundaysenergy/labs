<?php

/**
 * Domain provider ui controller
 */
abstract class DomainProviderUI {

  /**
   * Domain provider
   * @var DomainProvider
   */
  protected $provider;

  public function __construct($provider) {
    $this->provider = $provider;
  }

  /**
   * Build configuration form
   */
  public abstract function buildConfigForm(&$form, &$form_state);

  /**
   * Validate configuration form
   */
  public abstract function validateConfigForm($form, &$form_state);

  /**
   * Save configuration form
   */
  public abstract function saveConfigForm($form, &$form_state);
}
