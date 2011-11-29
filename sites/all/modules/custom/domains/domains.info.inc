<?php

/**
 * @file
 * Provides metadata for the domain entity
 */

/**
 * Implements hook_entity_property_info()
 */
function domains_entity_property_info() {
  $info = array();

  // Add meta-data about the basic domain properties.
  $properties = &$info['domains_domain']['properties'];

  $properties['id'] = array(
    'label' => t('Domain ID'),
    'description' => t('The internal numeric ID of the domain.'),
    'type' => 'integer',
    'schema field' => 'id',
  );
  $properties['name'] = array(
    'label' => t('Domain Name'),
    'description' => t('The unique domain name.'),
    'type' => 'text',
    'setter callback' => 'entity_property_verbatim_set',
    'required' => TRUE,
    'schema field' => 'name',
  );
  $properties['controller'] = array(
    'label' => t('Domain Controller'),
    'description' => t('The domain controller ID.'),
    'type' => 'text',
    'setter callback' => 'entity_property_verbatim_set',
    'required' => TRUE,
    'schema field' => 'controller',
  );
  $properties['expiration'] = array(
    'label' => t('Expiration date'),
    'description' => t('The expiration date of the domain.'),
    'type' => 'date',
    'setter callback' => 'entity_property_verbatim_set',
    'schema field' => 'expiration',
  );
  $properties['uid'] = array(
    'label' => t("Creator ID"),
    'type' => 'integer',
    'description' => t("The unique ID of the domain creator."),
    'setter callback' => 'entity_property_verbatim_set',
    'clear' => array('creator'),
    'schema field' => 'uid',
  );
  $properties['creator'] = array(
    'label' => t("Creator"),
    'type' => 'user',
    'description' => t("The creator of the product."),
    'getter callback' => 'domains_domain_get_properties',
    'setter callback' => 'domains_domain_set_properties',
    'required' => TRUE,
    'clear' => array('uid'),
  );

  return $info;
}
