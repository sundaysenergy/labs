<?php

/**
 * @file
 * Admin settings for the API
 */

function wsclient_dynect_admin_settings($form, $from_state) {
  $form['wsclient_dynect_customer_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Customer Name'),
    '#default_value' => variable_get('wsclient_dynect_customer_name', ''),
  );
  $form['wsclient_dynect_user_name'] = array(
    '#type' => 'textfield',
    '#title' => t('User name'),
    '#default_value' => variable_get('wsclient_dynect_user_name', ''),
  );
  $form['wsclient_dynect_password'] = array(
    '#type' => 'password',
    '#title' => t('Password'),
    '#default_value' => variable_get('wsclient_dynect_password', ''),
  );
  return system_settings_form($form);
}
