<?php

require 'scripts/misc.php';


/**
 * @file
 * Display a list of experimetns in a table
 */

/**
 * Build the experiments table list.
 *
 * @return
 *   A form array set for theming by theme_mecsim_experiments_form()
 */
function mecsim_experiments_form($form, $form_state) {
  // Identify that the elements in 'experiments' are a collection, to
  // prevent Form API from flattening the array when submitted.
  $form['experiments']['#tree'] = TRUE;

  //****************************
  global $user;
  //query the table mecsim for the experiments that belong to the current user
  $result = db_query('SELECT m.exp_number, m.name, m.description, m.status, m.nimrodid, m.creation_time, m.exp_location 
                      FROM {mecsim} m where m.owner=:username', array(':username'=>$user->name));
  
  // Iterate through each database result.
  foreach ($result as $item) {
    $form['experiments'][$item->exp_number] = array(
      // title
      'title' => array(
        '#markup' => check_plain($item->name),
      ),

      // status
      'status' => array(
        '#markup' => check_plain(getStatusString($item->status)),
      ),

      //description
      'description' => array(
        '#markup' => check_plain($item->description),
      ),

      //created on
      'createddate' => array(
        '#markup' => date('Y-m-d H:i:s', $item->creation_time),
      ),
      //new design, dont need this two columns
      // //experiment parameter
      // 'experiment_params' => array(
      //   '#type' => 'submit',
      //   '#value' => 'Edit',
      //   '#name' => 'experiment_'.$item->id,
      // ),
      // //simulation parameters
      // 'simulation_params' => array(
      //   '#type' => 'submit',
      //   '#value' => 'Edit',
      //   '#name' => 'simulation_'.$item->id,
      // ),
      //),
      //adminstration
      'administration' => array(
        array('#type' => 'submit',
          '#value' => 'Edit',
          '#name' => 'edit_'.$item->exp_number,  // not the best way, but it works. TODO: fix this
        ),
        array('#type' => 'submit',
          '#value' => 'Delete',
          '#name' => 'delete_'.$item->exp_number,  
        ),
        ),
     );
    // add to job_management buttons based on the status of the experiment
    switch($item->status){
      case Status::NotSubmitted:
        $form['experiments'][$item->exp_number]['job_management']=array(
          '#type' => 'submit',
          '#value' => 'Submit',
          '#name' => 'submit_'.$item->exp_number,
        );
        break;
      case Status::Submitted:
        $form['experiments'][$item->exp_number]['job_management']=array(
          '#type' => 'submit',
          '#value' => 'Run',
          '#name' => 'run_'.$item->exp_number,
        );
        break;
      case Status::Running:
        $form['experiments'][$item->exp_number]['job_management']=array(
          array(
            '#type' => 'submit',
            '#value' => 'Pause',
            '#name' => 'pause_'.$item->exp_number,
          ),
          array(
            '#type' => 'submit',
            '#value' => 'Stop',
            '#name' => 'stop_'.$item->exp_number,
          ),
        );
        break;
      case Status::Pause:
        $form['experiments'][$item->exp_number]['job_management']=array(
            '#type' => 'submit',
            '#value' => 'Resume',
            '#name' => 'resume_'.$item->exp_number,
          );        
        break;
      case Status::Stop:
        $form['experiments'][$item->exp_number]['job_management']=array(
            '#type' => 'submit',
            '#value' => 'Run',
            '#name' => 'run_'.$item->exp_number,
          );        
        break;  
    }//end switch

   }


  // Now we add our submit button, for submitting the form results.
  //
  $form['newexperiment'] = array('#type' => 'submit', 
                                 '#value' => 'New Experiment',
                                 '#name' => 'Create',
                                );
  
  return $form;
}

/**
 * Theme callback for the mecsim_experiments_form form.
 *
 * The theme callback will format the $form data structure into a table and
 * add our tabledrag functionality.  (Note that drupal_add_tabledrag should be
 * called from the theme layer, and not from a form declaration.  This helps
 * keep template files clean and readable, and prevents tabledrag.js from
 * being added twice accidently.
 *
 * @return
 *   The rendered tabledrag form
 */
function theme_mecsim_experiments_form($variables) {
  $form = $variables['form'];

  // Initialize the variable which will store our table rows.
  $rows = array();

  // Iterate over each element in our $form['experiments'] array.
  foreach (element_children($form['experiments']) as $id) {

    // We are now ready to add each element of our $form data to the $rows
    // array, so that they end up as individual table cells when rendered
    // in the final table.  We run each element through the drupal_render()
    // function to generate the final html markup for that element.
    $rows[] = array(
      'data' => array(
        // Add our 'name' column.
        drupal_render($form['experiments'][$id]['title']),
        // Add our 'description' column.
        drupal_render($form['experiments'][$id]['description']),
        // Add our 'status' column.
        drupal_render($form['experiments'][$id]['status']),
        // Add our 'date created' column.
        drupal_render($form['experiments'][$id]['createddate']),
        // // Add our 'date created' column.
        // drupal_render($form['experiments'][$id]['experiment_params']),
        // // Add our 'date created' column.
        // drupal_render($form['experiments'][$id]['simulation_params']),
        // Add our 'date created' column.
        drupal_render($form['experiments'][$id]['job_management']),
        // Add our 'date created' column.
        drupal_render($form['experiments'][$id]['administration']),
      ),
    );
  }

  // We now define the table header values.  Ensure that the 'header' count
  // matches the final column count for your table.
  $header = array(t('Title'), t('Description'),t('Status'), t('Created On'), 
                  //t('Experiments Parameters'), t('Simulation Parameters'), 
                  t('Job Management'), t('Administration'));

  $table_id = 'experiments-table';

  $output = theme('table', array(
    'header' => $header,
    'rows' => $rows,
    'attributes' => array('id' => $table_id),
  ));
  $output .= drupal_render_children($form);

  return $output;
}

/**
 * Submit callback for the mecsim_experiments_form_submit form.
 *
 * Updates the 'weight' column for each element in our table, taking into
 * account that item's new order after the drag and drop actions have been
 * performed.
 */
function mecsim_experiments_form_submit($form, &$form_state) {
    //drupal_set_message("...................item:".$form_state['clicked_button']['#name']);
  $button_name = $form_state['clicked_button']['#name'];
  if($button_name === 'Create'){
    $_SESSION['mecsim_experiment']= array(
      'id' => '-1',  
    );
    $form_state['redirect'] = 'newexperiment';
  }
  else{
    $values = explode("_", $button_name);
    //button name = first part, id = last part
    $id = $values[count($values) - 1];
    $button_value = "";
    for($i=0; $i< count($values)-1; $i++){
      $button_value = $button_value.$values[$i];
    }
    $_SESSION['mecsim_experiment']= array(
      'id' => $id,  
    );
    // goes to this page
    // change the settings for this experiment
    if($button_value === 'edit'){
      $form_state['redirect'] = 'mecsim/experiment';
    }
    elseif($button_value === 'delete'){
      deleteExperiment($id);
      $_SESSION['mecsim_experiment']['id']= -1;
    }    
    elseif($button_value === 'run'){
      drupal_set_message('Run');
      $form_state['redirect'] = 'mecsim';
    }
    elseif($button_value === 'stop'){
      drupal_set_message('Stop');
      $form_state['redirect'] = 'mecsim';    
    }
    elseif($button_value === 'pause'){
      drupal_set_message('Pause');
      $form_state['redirect'] = 'mecsim';    
    }
    elseif($button_value === 'resume'){
      drupal_set_message('Resume');
      $form_state['redirect'] = 'mecsim';    
    }
    elseif($button_value === 'submit'){
      drupal_set_message('Submit');
      //when submit
      //create the folder, and get all the data
      //submit whatever the experiment passes to nimrod
      $form_state['redirect'] = 'mecsim';    
    }

  }//end else


}


