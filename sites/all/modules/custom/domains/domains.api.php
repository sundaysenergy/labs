<?php
/**
 * @file
 * Domains.module's API
 */

/**
 * Implements this hook to provide your controler for your domain provider.
 * 
 * Return array of domain providers, each element is keyed-array, with
 *  $element['class'] is name of class implemented DomainProviderInterface.
 */
function hook_domains_provider_info() {
  return array(
    'enom' => array(
      'label' => 'Enom',
      'class' => 'EnomDomainController',
      'ui class' => 'EnomDomainUiController',
    ),
  );
}
