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
   * @param string $data
   *  The data that should be unserialized.
   * @return mixed
   *  The unserialized data.
   */
  public function unserialize($data) {
    // Awesome, as long as it works.  Enom text response.
    $values = parse_ini_string($data);
    if (is_array($values)) {
      foreach ($values as $key => $val) {
        // the last character of the string
        $last = substr($key, -1, 1);

        // THIS IS CUSTOM UNSERIALIZE BEHAVIOR
        // add all values of keys that end in a number into a single key with array value
        if (is_numeric($last)) {
          // make multi vals into array instead
          $main_key = substr($key, 0, -1);
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
