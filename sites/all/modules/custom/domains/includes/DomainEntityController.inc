<?php

/**
 * Domain entity controller class
 */
class DomainEntityController extends EntityAPIControllerExportable {

  public function buildContent($entity, $view_mode = 'full', $langcode = NULL, $content = array()) {
    $content['expiration'] = array(
      '#type' => 'item',
      '#title' => t('Expiration Date'),
      '#markup' => format_date($entity->expiration),
    );

    return parent::buildContent($entity, $view_mode, $langcode, $content);
  }
}
