//https://github.com/patmcgee1983/piSwitchServer
var hideMessageTimer;
var nr=0;

function showError(text)
{

  icon = "glyphicon glyphicon-remove";
  bgcolor = "#f73838";

  showMessage(text,icon,bgcolor)
}

function showSuccess(text)
{
  icon = "glyphicon glyphicon-ok";
  bgcolor = "#008916";
  showMessage(text,icon,bgcolor)
}

function showInfo(text)
{
  icon = "glyphicon glyphicon-info-sign";
  bgcolor = "#000000";
  showMessage(text,icon,bgcolor)
}

function showMessage(text,icon,bgcolor)
{
	$("#messageBox").hide("slide", { direction: "down" }, 500, function()
	{
		clearTimeout(hideMessageTimer);
		$("#messageBox_inner").css('background-color', bgcolor);
		$("#messageBox_inner").html('<font size=\"+2\"><span class="'+icon+'"></span> '+text+'</font>')
		$("#messageBox").show("slide", { direction: "down" }, 500);
		$("#messageBox").on("click", function()
		{
			hideMessage();
		});

		hideMessageTimer = setTimeout(function()
		{
			hideMessage();
		}, 5000);
	});
}

function hideMessage()
{
	$("#messageBox").hide("slide", { direction: "down" }, 500);
}

function ValidateIPaddress(ipaddress) {
  if (/^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$/.test(ipaddress)) {
    return (true)
  }
  return (false)
}



var url


function disableEdit(prog)
{
  var i=0
  $('input[name="chk_days'+prog+'[]"]').each(function(){
    $(this).attr("disabled","disabled")
    i++
  });

  $("#txt_name_prog"+prog).attr("disabled","disabled")
  $("#startTime_prog"+prog).attr("disabled","disabled")

  $("#chk_days_m_prog"+prog).attr("disabled","disabled")
  $("#btn_save_prog"+prog).attr("disabled","disabled")
  $("#btn_cancel_prog"+prog).attr("disabled","disabled")



}



function enableEdit(prog)
{
  console.log("enable edit " + prog)

  $('input[name="chk_days'+prog+'[]"]').each(function(){
    $(this).removeAttr("disabled")
  });


  $("#txt_name_prog"+prog).removeAttr("disabled")
  $("#startTime_prog"+prog).removeAttr("disabled")
  $("#btn_save_prog"+prog).removeAttr("disabled")
  $("#btn_cancel_prog"+prog).removeAttr("disabled")



}


function clickOn(nr)
{
  timers[nr].Mode = 1;
  timers[nr].ReleName = $("#txt_name_rele"+nr).val();
  $("#btn_auto_prog"+nr).attr('class', 'btn btn-secondary btn-lg');
  $("#btn_off_prog"+nr).attr('class', 'btn btn-secondary btn-lg');
  $("#btn_on_prog"+nr).attr('class', 'btn btn btn-success btn-lg');
  savemode();
  }

  function clickOff(nr)
  {
      timers[nr].Mode = 0;
      timers[nr].ReleName = $("#txt_name_rele"+nr).val();
      $("#btn_auto_prog"+nr).attr('class', 'btn btn-secondary btn-lg');
      $("#btn_off_prog"+nr).attr('class', 'btn btn-danger btn-lg');
      $("#btn_on_prog"+nr).attr('class', 'btn btn-secondary btn-lg');
      savemode()
    }

    function clickAuto(nr)
    {
        timers[nr].Mode = 2;
        timers[nr].ReleName = $("#txt_name_rele"+nr).val();
        $("#btn_auto_prog"+nr).attr('class', 'btn btn-success btn-lg');
        $("#btn_off_prog"+nr).attr('class', 'btn btn-secondary btn-lg');
        $("#btn_on_prog"+nr).attr('class', 'btn btn-secondary btn-lg');
        savemode();
      }

      function savemode(){
        //timers[nr].ReleName = $("#txt_name_rele"+nr).val(); ya está leido en los click
        $.post("savemode?cmd=" + 'savemode' +
          "&nr=" + nr+
          "&id=" + timers[nr].devID+
          "&name=" + timers[nr].ReleName+
          "&mode=" + timers[nr].Mode
          ).done(function(data){
            if ( data.success === "1" ) {
              showSuccess("Datos grabados");
              loadData();
            } else {
                showError("error grabando los datos")
            }
          }).fail(function(err){
            console.log("err saving data " + JSON.stringify(err));
          });

      }

    function on(nr)
    {
      $("#btn_auto_prog"+nr).attr('class', 'btn btn-secondary btn-lg');
      $("#btn_off_prog"+nr).attr('class', 'btn btn-secondary btn-lg');
      $("#btn_on_prog"+nr).attr('class', 'btn btn btn-success btn-lg');
    }

    function off(nr)
    {
      $("#btn_auto_prog"+nr).attr('class', 'btn btn-secondary btn-lg');
      $("#btn_off_prog"+nr).attr('class', 'btn btn-danger btn-lg');
      $("#btn_on_prog"+nr).attr('class', 'btn btn-secondary btn-lg');
    }

    function auto(nr)
    {
        $("#btn_auto_prog"+nr).attr('class', 'btn btn-success btn-lg');
        $("#btn_off_prog"+nr).attr('class', 'btn btn-secondary btn-lg');
        $("#btn_on_prog"+nr).attr('class', 'btn btn-secondary btn-lg');
    }


  function save_d(nr,i)    //para guardar los datos en el nodeMCU para puesta por defecto
            {
              showSuccess("Tratando de grabar datos...");

              j=0;
              days = 0;
              for(j=0;j<7;j++){
                if(  timers[nr].prog[i].days[j]==true){
                  days += Math.pow(2,j);
                  }
              }
                    $.post("save?cmd=" + 'save' +
                      "&nr=" + nr+
                      "&id=" + timers[nr].prog[i].progID+
                      "&name=" + timers[nr].prog[i].name+
                      "&startTime=" + timers[nr].prog[i].startTime+
                      "&endTime=" + timers[nr].prog[i].endTime+
                      "&days=" + days+
                      "&sun_set="+timers[nr].prog[i].sun_set+
                      "&sign_delay="+timers[nr].prog[i].delay+
                      "&onoff="+timers[nr].prog[i].onoff+
                      "&delay_time="+timers[nr].prog[i].delay_time+
                      "&aleatorio="+timers[nr].prog[i].aleatorio
                      ).done(function(data){
                        if ( data.success === "1" ) {
                          showSuccess("Datos grabados");
                          loadData();
                        } else {
                            showError("error grabando los datos")
                        }
                      }).fail(function(err){
                        //console.log("err saving data " + JSON.stringify(err));
                      });
                      disableEdit(i)
              }


        function update(i)    //guardar los datos en el nodeMCU
        {
          console.log("Savind data to server..."+i)
          showSuccess("Tratando de grabar datos...");
          timers[nr].prog[i].name = $("#txt_name_prog"+i).val()
          timers[nr].prog[i].startTime = $("#startTime_prog"+i).val()
          timers[nr].prog[i].endTime = $("#endTime_prog"+i).val()
          timers[nr].prog[i].aleatorio = $("#aleatorio_prog"+i).val()
          //d = document.getElementById("select_id").value;

          j=0;
          days = 0;

          $('input[name="chk_days'+i+'[]"]').each(function(){
            timers[nr].prog[i].days[j]=0;
              if ($(this).prop('checked') == true)
            {
              days += Math.pow(2,j);
              timers[nr].prog[i].days[j]=1;
              }
            j++
          });
                $.post("save?cmd=" + 'save' +
                  "&nr=" + nr+
                  "&id=" + timers[nr].prog[i].progID+
                  "&name=" + timers[nr].prog[i].name+
                  "&startTime=" + timers[nr].prog[i].startTime+
                  "&endTime=" + timers[nr].prog[i].endTime+
                  "&days=" + days+
                  "&sun_set="+timers[nr].prog[i].sun_set+
                  "&sign_delay="+timers[nr].prog[i].delay+
                  "&onoff="+timers[nr].prog[i].onoff+
                  "&delay_time="+timers[nr].prog[i].delay_time+
                  "&aleatorio="+timers[nr].prog[i].aleatorio

                  ).done(function(data){
                    if ( data.success === "1" ) {
                      showSuccess("Datos grabados");
                      loadData();
                    } else {
                        showError("error grabando los datos")
                    }
                  }).fail(function(err){
                    console.log("err saving data " + JSON.stringify(err));
                  });
                  disableEdit(i)
          }

          function loadData(jsonString)
          {
              $("#contentA").empty()
//primero los botones de on off y Auto
                name = timers[nr].ReleName;
                sunrise=conf["sunrise"]; // en conf vienen las horas de salida y puesta del sol
                sunset=conf["sunset"];

                str = "";
                str += "<table style=\"width: 100%\">"
                str += "<tr>"
                str += "<td colspan=\"5\"><input type=\"text\" id=\"txt_name_rele"+nr+"\" style=\"font-size: 24px; background: #ffffff; border: 0\" value=\""+name+"\"></h2></td></tr>"
                str += "<tr><td style=\"margin: 0\"><button type=\"button\" id=\"btn_on_prog"+nr+"\" class=\"btn btn-secondary btn-lg\">On</button></td>"
                str += "<td style=\"padding-left: 10px\"><button type=\"button\" class=\"btn btn-secondary btn-lg\" id=\"btn_off_prog"+nr+"\">Off</button></td>"
                str += "<td style=\"padding-left: 10px\"><button type=\"button\" class=\"btn btn-secondary btn-lg\" id=\"btn_auto_prog"+nr+"\">Auto</button></td>"
                str +="<td>(UTC) Sunrise <span id='sunrise'>"+sunrise+"</span></td>"
                str +="<td>Sunset <span id='sunset'>"+sunset+"</span></td>"
                str += "</tr></table></td>"
                str += "<hr>"

              $("#contentA").append("<div id=\"Configuracion"+nr+"\">"+str+"</div>")




///////////////////////////////////////

                  if (timers[nr].Mode == 1)
                  {
                    on(nr);
                  }
                  else if (timers[nr].Mode == 0)
                  {
                    off(nr);
                  }else{
                    auto(nr);
                  }


            (function(nr) {
              $("#btn_auto_prog"+nr).on("click", function()
              {
                clickAuto(nr);
              });
              $("#btn_on_prog"+nr).on("click", function()
              {
                clickOn(nr);
              })
              $("#btn_off_prog"+nr).on("click", function()
              {
                clickOff(nr);
              });
            })(nr);


//////////////////////////////////////


            for (p=0; p<timers[nr].prog.length; p++)
            {
              name = timers[nr].prog[p].name;
              id = timers[nr].prog[p].progID;//progs[i].id
              startTime = String(timers[nr].prog[p].startTime);//progs[i].startTime
              endTime = String(timers[nr].prog[p].endTime); //por si viene un numero sin :
              aleatorio = String(timers[nr].prog[p].aleatorio);  //para un aleatorio en el tiempo hh:mm
              sun_set = String(timers[nr].prog[p].sunset); //0:time, 1=sunrise, 2=sunset
              sign_delay = timers[nr].prog[p].sign_delay; //positivo o negativo
              onoff = timers[nr].prog[p].onoff; // on off en caso de select sunrise sunset
              delay_time=timers[nr].prog[p].delay_time;


              state = timers[nr].Mode;// 0 off 1 on 2 auto
              elementId = p;
            // Inserting a 0 in front of start time if needed
              testHour = startTime.split(":")
              if (testHour[0].length < 2)
              {
                startTime = "0"+startTime
              }

              // Inserting a 0 in front of end time if needed
              testHour = endTime.split(":")
              if (testHour[0].length < 2)
              {
                endTime = "0"+endTime
              }

              str = "";
              str += "<table style=\"width: 100%\">"
              str += "<tr>"
              str += "<td colspan=\"5\"><input type=\"text\" id=\"txt_name_prog"+elementId+"\" style=\"font-size: 24px; background: #ffffff; border: 0\" value=\""+name+"\"></h2></td>"
              str +="</tr><tr>"



              str +=" <td>"
              str +="         <label class='radio-inline'><input type='radio' id='optradio1"+elementId+"' name='optradio"+elementId+"'>Time</label>"
              str +="         <label class='radio-inline'><input type='radio' id='optradio2"+elementId+"' name='optradio"+elementId+"'>Sunrise</label>"
              str +="         <label class='radio-inline'><input type='radio' id='optradio3"+elementId+"' name='optradio"+elementId+"'>Sunset</label>"
              str +="</td>"


              str +=" <td>"

              str +=  "<select  id='onoff"+elementId+"'>"
              str +=  "<option value=0 selected>OFF</option>"
              str +=  "<option value=1>ON</option></select>"

              str +="</td>"


              str +=" <td>"

              str +="<select  id='delay"+elementId+"'>"
              str +=  "<option value=0 selected>Delay +</option>"
              str +=  "<option value=1>Delay -</option></select>"

              str +="</td>"

              str += "<td><span id=\"delay_time_span"+elementId+"\">Delay: <input type=\"time\" id=\"delay_time"+elementId+"\" value=\""+delay_time+"\"></span></td>"
              str += "<td style=\"padding-left: 20px\"><span id=\"startTime_span"+elementId+"\">Start Time: <input type=\"time\" id=\"startTime_prog"+elementId+"\" value=\""+startTime+"\"></span></td>"
              str += "<td>Aleatorio +/-: <input type=\"time\" id=\"aleatorio_prog"+elementId+"\" value=\""+aleatorio+"\"></td>"


              str +="</tr>"
    //          str += "<td style=\"padding-left: 20px\"><span id=\"startTime_span"+elementId+"\">Start Time: <input type=\"time\" id=\"startTime_prog"+elementId+"\" value=\""+startTime+"\"></span></td>"
              str += "<td><span id=\"endTime_span"+elementId+"\">End Time: <input type=\"time\" id=\"endTime_prog"+elementId+"\" value=\""+endTime+"\"></span></td>"
              str += "<td style=\"padding-left: 10px\"><table style=\"display: inline\" border=\"0\"><tr><td>M</td><td>T</td><td>W</td><td>Th</td><td>F</td><td>S</td><td>Su</td></tr>"
              str += "<tr><td><input type=\"checkbox\" id=\"chk_days"+elementId+"\" name=\"chk_days"+elementId+"[]\"></td>"
              str += "<td><input type=\"checkbox\" id=\"chk_days"+elementId+"\" name=\"chk_days"+elementId+"[]\"></td>"
              str += "<td><input type=\"checkbox\" id=\"chk_days"+elementId+"\" name=\"chk_days"+elementId+"[]\"></td>"
              str += "<td><input type=\"checkbox\" id=\"chk_days"+elementId+"\" name=\"chk_days"+elementId+"[]\"></td>"
              str += "<td><input type=\"checkbox\" id=\"chk_days"+elementId+"\" name=\"chk_days"+elementId+"[]\"></td>"
              str += "<td><input type=\"checkbox\" id=\"chk_days"+elementId+"\" name=\"chk_days"+elementId+"[]\"></td>"
              str += "<td><input type=\"checkbox\" id=\"chk_days"+elementId+"\" name=\"chk_days"+elementId+"[]\"></td></tr></table>"

              str += "<td style=\"padding-left: 10px\"><button type=\"button\" class=\"btn btn-secondary\" id=\"btn_edit_prog"+elementId+"\">Edit</button></td>"
              str += "<td style=\"padding-left: 10px\"><button type=\"button\" class=\"btn btn-secondary\" id=\"btn_save_prog"+elementId+"\">Save</button></td>"
              str += "<td style=\"padding-left: 10px\"><button type=\"button\" class=\"btn btn-secondary\" id=\"btn_cancel_prog"+elementId+"\">Cancel</button></td>"

              str += "</tr></table></td>"
              str += "<hr>"
              $("#contentA").append("<div id=\"prog"+elementId+"\">"+str+"</div>")

              var j=0
              $('input[name="chk_days'+elementId+'[]"]').each(function(){ //dias de la semana
              if (timers[nr].prog[p].days[j]==1)
                {
                    $(this).attr('checked', true);
                }
                j++
              });



              (function(elementId) {
                //set valores iniciales
                  $("#delay"+elementId).val(sign_delay);
                  $("#onoff"+elementId).val(onoff);
                  $("#delay_time"+elementId).val(delay_time);
                  $("#endTime_span"+elementId).hide();
                  //$("#endTime_span").toggleClass("display-none");
                  //$("#startTime_span"+elementId).show();

                  if ( timers[nr].prog[elementId].sun_set==0){
                    $("#optradio1"+elementId).attr('checked', 'checked');
                    $("#optradio1"+elementId).click();
                    $("#delay"+elementId).hide();
                    $("#startTime_span"+elementId).show();
                    $("#delay_time_span"+elementId).hide();
                    }
                  if ( timers[nr].prog[elementId].sun_set==1){
                    $("#optradio2"+elementId).attr('checked', 'checked');
                    $("#optradio2"+elementId).click();
                    $("#delay"+elementId).show();
                    $("#startTime_span"+elementId).hide();
                    $("#delay_time_span"+elementId).show();
                    }
                  if ( timers[nr].prog[elementId].sun_set==2){
                    $("#optradio3"+elementId).attr('checked', 'checked');
                    $("#optradio3"+elementId).click();
                    $("#delay"+elementId).show();


                    $("#startTime_span"+elementId).hide();
                    $("#delay_time_span"+elementId).show();
                    }


                $("#btn_cancel_prog"+elementId).on("click", function()
                {
                  disableEdit(elementId)
                })
                $("#btn_edit_prog"+elementId).on("click", function()
                {
                    enableEdit(elementId)
                })
                $("#btn_save_prog"+elementId).on("click", function()
                {
                  update(elementId)
                })

                $("#optradio1"+elementId).on("click", function()
                {
                  $("#delay"+elementId).hide();
                  $("#startTime_span"+elementId).show();
                  $("#delay_time_span"+elementId).hide();
                  timers[nr].prog[elementId].sun_set=0;
                })
                $("#optradio2"+elementId).on("click", function()
                {
                  $("#delay"+elementId).show();
                  $("#startTime_span"+elementId).hide();
                  $("#delay_time_span"+elementId).show();
                  timers[nr].prog[elementId].sun_set=1;
                })
                $("#optradio3"+elementId).on("click", function()
                {
                  $("#delay"+elementId).show();


                  $("#startTime_span"+elementId).hide();
                  $("#delay_time_span"+elementId).show();
                  timers[nr].prog[elementId].sun_set=2;
                })

                $("#onoff"+elementId).change(function(){
                  console.log('Selected value: ' + $(this).val());
                    timers[nr].prog[elementId].onoff= $(this).val();
                });

                $("#delay"+elementId).change(function(){
                    timers[nr].prog[elementId].delay= $(this).val();
                });


              })(p);
              disableEdit(elementId)
            }
          }


          function getData()
          {
            $("#contentA").empty()
            piIp = $("#piIp").val()
            //url = "http://" + piIp + ":5000"
            url = "http://192.168.1.84";
            console.log("sending ajax request to " + url)
            console.log(piIp);

            if (piIp == "test")
            {
              showSuccess("Using test interface")
              var jsonString='[{"devName":"Sistema3_ReleD5","ReleName":"ReleNameD5","devID":"0","Mode":0,"prog":[{"sun_set":"0","onoff":"0","aleatorio":"00:00","sign_delay":"0","delay_time":"00:00","name":"Prog0_DevID5","progID":0,"startTime":"08:12","endTime":"19:34","days":["1","1","1","1","1","1","1"]},{"sun_set":"0","onoff":"0","aleatorio":"00:00","sign_delay":"0","delay_time":"00:00","name":"Prog1_DevID5","progID":1,"startTime":"08:12","endTime":"19:34","days":["0","0","0","0","0","0","0"]},{"sun_set":"0","onoff":"0","aleatorio":"00:00","sign_delay":"0","delay_time":"00:00","name":"Prog2_DevID5","progID":2,"startTime":"08:12","endTime":"19:34","days":["0","0","0","0","0","0","0"]},{"sun_set":"0","onoff":"0","aleatorio":"00:00","sign_delay":"0","delay_time":"00:00","name":"Prog3_DevID5","progID":3,"startTime":"08:12","endTime":"19:34","days":["0","0","0","0","0","0","0"]},{"sun_set":"0","onoff":"0","aleatorio":"00:00","sign_delay":"0","delay_time":"00:00","name":"Prog4_DevID5","progID":4,"startTime":"08:12","endTime":"19:34","days":["0","0","0","0","0","0","0"]},{"sun_set":"0","onoff":"0","aleatorio":"00:00","sign_delay":"0","delay_time":"00:00","name":"Prog5_DevID5","progID":5,"startTime":"08:12","endTime":"19:34","days":["0","0","0","0","0","0","0"]},{"sun_set":"0","onoff":"0","aleatorio":"00:00","sign_delay":"0","delay_time":"00:00","name":"Prog6_DevID5","progID":6,"startTime":"08:12","endTime":"19:34","days":["0","0","0","0","0","0","0"]}]},{"devName":"Sistema3_ReleD6","ReleName":"ReleD6","devID":"1","Mode":0,"prog":[{"sun_set":"0","onoff":"0","aleatorio":"00:00","sign_delay":"0","delay_time":"00:00","name":"Prog0_DevID6","progID":0,"startTime":"08:12","endTime":"19:34","days":["1","1","1","1","1","1","1"]},{"sun_set":"0","onoff":"0","aleatorio":"00:00","sign_delay":"0","delay_time":"00:00","name":"Prog1_DevID6","progID":1,"startTime":"08:12","endTime":"19:34","days":["0","0","0","0","0","0","0"]},{"sun_set":"0","onoff":"0","aleatorio":"00:00","sign_delay":"0","delay_time":"00:00","name":"Prog2_DevID6","progID":2,"startTime":"08:12","endTime":"19:34","days":["0","0","0","0","0","0","0"]},{"sun_set":"0","onoff":"0","aleatorio":"00:00","sign_delay":"0","delay_time":"00:00","name":"Prog3_DevID6","progID":3,"startTime":"08:12","endTime":"19:34","days":["0","0","0","0","0","0","0"]},{"sun_set":"0","onoff":"0","aleatorio":"00:00","sign_delay":"0","delay_time":"00:00","name":"Prog4_DevID6","progID":4,"startTime":"08:12","endTime":"19:34","days":["0","0","0","0","0","0","0"]},{"sun_set":"0","onoff":"0","aleatorio":"00:00","sign_delay":"0","delay_time":"00:00","name":"Prog5_DevID6","progID":5,"startTime":"08:12","endTime":"19:34","days":["0","0","0","0","0","0","0"]},{"sun_set":"0","onoff":"0","aleatorio":"00:00","sign_delay":"0","delay_time":"00:00","name":"Prog6_DevID6","progID":6,"startTime":"08:12","endTime":"19:34","days":["0","0","0","0","0","0","0"]}]},{"devName":"Sistema3_ReleD7","ReleName":"ReleD7","devID":"2","Mode":0,"prog":[{"sun_set":"0","onoff":"0","aleatorio":"00:00","sign_delay":"0","delay_time":"00:00","name":"Prog0_DevID7","progID":0,"startTime":"08:12","endTime":"19:34","days":["1","1","1","1","1","1","1"]},{"sun_set":"0","onoff":"0","aleatorio":"00:00","sign_delay":"0","delay_time":"00:00","name":"Prog1_DevID7","progID":1,"startTime":"08:12","endTime":"19:34","days":["0","0","0","0","0","0","0"]},{"sun_set":"0","onoff":"0","aleatorio":"00:00","sign_delay":"0","delay_time":"00:00","name":"Prog2_DevID7","progID":2,"startTime":"08:12","endTime":"19:34","days":["0","0","0","0","0","0","0"]},{"sun_set":"0","onoff":"0","aleatorio":"00:00","sign_delay":"0","delay_time":"00:00","name":"Prog3_DevID7","progID":3,"startTime":"08:12","endTime":"19:34","days":["0","0","0","0","0","0","0"]},{"sun_set":"0","onoff":"0","aleatorio":"00:00","sign_delay":"0","delay_time":"00:00","name":"Prog4_DevID7","progID":4,"startTime":"08:12","endTime":"19:34","days":["0","0","0","0","0","0","0"]},{"sun_set":"0","onoff":"0","aleatorio":"00:00","sign_delay":"0","delay_time":"00:00","name":"Prog5_DevID7","progID":5,"startTime":"08:12","endTime":"19:34","days":["0","0","0","0","0","0","0"]},{"sun_set":"0","onoff":"0","aleatorio":"00:00","sign_delay":"0","delay_time":"00:00","name":"Prog6_DevID7","progID":6,"startTime":"08:12","endTime":"19:34","days":["0","0","0","0","0","0","0"]}]},{"devName":"Sistema3_ReleD8","ReleName":"ReleD8","devID":"3","Mode":0,"prog":[{"sun_set":"0","onoff":"0","aleatorio":"00:00","sign_delay":"0","delay_time":"00:00","name":"Prog0_DevID8","progID":0,"startTime":"08:12","endTime":"19:34","days":["1","1","1","1","1","1","1"]},{"sun_set":"0","onoff":"0","aleatorio":"00:00","sign_delay":"0","delay_time":"00:00","name":"Prog1_DevID8","progID":1,"startTime":"08:12","endTime":"19:34","days":["0","0","0","0","0","0","0"]},{"sun_set":"0","onoff":"0","aleatorio":"00:00","sign_delay":"0","delay_time":"00:00","name":"Prog2_DevID8","progID":2,"startTime":"08:12","endTime":"19:34","days":["0","0","0","0","0","0","0"]},{"sun_set":"0","onoff":"0","aleatorio":"00:00","sign_delay":"0","delay_time":"00:00","name":"Prog3_DevID8","progID":3,"startTime":"08:12","endTime":"19:34","days":["0","0","0","0","0","0","0"]},{"sun_set":"0","onoff":"0","aleatorio":"00:00","sign_delay":"0","delay_time":"00:00","name":"Prog4_DevID8","progID":4,"startTime":"08:12","endTime":"19:34","days":["0","0","0","0","0","0","0"]},{"sun_set":"0","onoff":"0","aleatorio":"00:00","sign_delay":"0","delay_time":"00:00","name":"Prog5_DevID8","progID":5,"startTime":"08:12","endTime":"19:34","days":["0","0","0","0","0","0","0"]},{"sun_set":"0","onoff":"0","aleatorio":"00:00","sign_delay":"0","delay_time":"00:00","name":"Prog6_DevID8","progID":6,"startTime":"08:12","endTime":"19:34","days":["0","0","0","0","0","0","0"]}]}]';
              timers=JSON.parse(jsonString);
              console.log("timers simulado");
              console.log(timers);
              loadData(jsonString);
            }
            else
            {
              if (conf_recibida==false){
                showError("Intentar mas tarde, config no recibida");
                	$("#mensajes").html("Intentar mas tarde, config no recibida");
              }

              if(piIp==conf["pwd"])
              {
                showSuccess("pwd correcta...");
                getConfig();
                loadData();
                showSuccess("Comunic OK");
              }else{
                showError("pwd inv&aacute;lido o config no recibida");
              }
          }
          }


           $( document ).ready(function() {
              $("#btn_go").on("click",function() //se pulsa el botón GO
              {
                console.log("getting data");
                getConfig(); //lee la configuración
                getData(); //dibuja los botones
                $("#but_defecto").show();
                $("#but_alloff").show();
                $("#mensajes").html("Espere a recargar configuración");
              });

              showError("Espere, se est&aacute;n cargando los datos del servidor");
              getConfig(); //lee la configuración inicialmente

              $('#D5b').on("click",function(){ nr=0;loadData();});
              $('#D6b').on("click",function(){ nr=1;loadData();});
              $('#D7b').on("click",function(){ nr=2;loadData();});
              $('#D8b').on("click",function(){ nr=3;loadData();});

              $("#messageBox").on("click", function() { hideMessage();});
            });

            function getConfig()
            {
              num_rele=0;
               receiveDataTimers(0); //los programas de los temporizadoes, lo llamamos 4 veces
               receiveDataTimers(1);receiveDataTimers(2);
               receiveDataTimers(3);receiveDataTimers(0);
               receiveDataConf(); // la configuración
            }
