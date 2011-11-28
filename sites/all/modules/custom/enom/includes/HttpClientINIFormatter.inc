<?php

/**
 * Class for handling config/ini style text strings.
 * Returns an array of key value pairs
 *
 * @author Kai Curry <kai.curry@sundaysenergy.com>
 */

class HttpClientINIFormatter implements HttpClientFormatter {
  /**
   * Serializes arbitrary data to the implemented format.
   *
   * @param mixed $data
   *  The data that should be serialized.
   * @return string
   *  The serialized data as a string.
   */
  public function serialize($data) {
    if (is_string($data)) {
      return $data;
    }
    $string = '';
    if (is_array($data)) {
      foreach($data as $key => $value) {
        $string .= $key . ' = ' . $value . '\n';
      }
    }
    return $string;
  }

  /**
   * Unserializes data in the implemented format.
   *
   * Awesome, as long as it works.  Created for Enom text response.
   *
   * @param string $data
   *  The data that should be unserialized.
   * @return mixed
   *  The unserialized data.
   */
  public function unserialize($data) {
    // Too bad drupal_parse_info_file() doesn't have option from string
    $values = parse_ini_string($data);
    if (is_array($values)) {
      foreach ($values as $key => $val) {
        // the last character of the string
        $last = preg_replace('/[^0-9]*(\d+)$/', '$1', $key);

        // THIS IS CUSTOM UNSERIALIZE BEHAVIOR
        // add all values of keys that end in a number into a single key with array value
        if (is_numeric($last)) {
          // make multi vals into array instead
          $main_key = substr($key, 0, -1 * strlen($last));

          // add key and make it an array with value
          if (!isset($values[$main_key])) {
            $values[$main_key] = array($val);
          }
          // key already found
          else {
            // if key without the number not an array, make new array with value
            if (!is_array($values[$main_key])) {
              $values[$main_key] = array($values[$main_key]);
            }
            // add this value to it
            $values[$main_key][] = $val;
          }
          // now remove the old key/val pair
          unset($values[$key]);
        }
        // END CUSTOM UNSERIALIZE BEHAVIOR

      }
      return $values;
    }
    else {
      // Data was messed up or empty
      throw new InvalidArgumentException('Response was malformed or contains no data.');
    }
  }

  public function mimeType() {
    return 'text/html';
  }
}

if (!function_exists('parse_ini_string')) {
  function parse_ini_string($str, $ProcessSections=false) {
      $lines  = explode("\n", $str);
      $return = Array();
      $inSect = false;
      foreach($lines as $line){
          $line = trim($line);
          if(!$line || $line[0] == "#" || $line[0] == ";")
              continue;
          if($line[0] == "[" && $endIdx = strpos($line, "]")){
              $inSect = substr($line, 1, $endIdx-1);
              continue;
          }
          if(!strpos($line, '=')) // (We don't use "=== false" because value 0 is not valid as well)
              continue;
          
          $tmp = explode("=", $line, 2);
          if($ProcessSections && $inSect)
              $return[$inSect][trim($tmp[0])] = ltrim($tmp[1]);
          else
              $return[trim($tmp[0])] = ltrim($tmp[1]);
      }
      return $return;
  }
}