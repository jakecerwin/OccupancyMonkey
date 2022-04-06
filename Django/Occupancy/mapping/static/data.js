function sanitize(s) {
  // Be sure to replace ampersand first
  return s.replace(/&/g, '&amp;')
    .replace(/</g, '&lt;')
    .replace(/>/g, '&gt;')
    .replace(/"/g, '&quot;')
}

function convertToSlug(Text)
{
    return Text
        .replace(/:/g,'-colon-')
        .replace(/\)/g,'-rparanthesis-')
        .replace(/\(/g,'-lparanthesis-')
        .replace(/ /g,'-space-')
        .replace(/\^/g,'-carrot-')
        ;
}

function displayError(message) {
  let errorElement = document.getElementById("error")
  errorElement.innerHTML = message
}

function getstatus() {
  let xhr = new XMLHttpRequest()
  xhr.onreadystatechange = function() {
      if (xhr.readyState != 4) return
      loadPage(xhr)
  }

  xhr.open("GET", "/get-status", true)
  xhr.send()
}


function loadPage(xhr) {
  if (xhr.status == 200) {
      let response = JSON.parse(xhr.responseText)
      loadStatus(response)
      return
  }

  if (xhr.status == 0) {
      displayError("Cannot connect to server")
      return
  }


  if (!xhr.getResponseHeader('content-type') == 'application/json') {
      displayError("Received status=" + xhr.status)
      return
  }

  let response = JSON.parse(xhr.responseText)
  if (response.hasOwnProperty('error')) {
      displayError(response.error)
      return
  }

  displayError(response)
}

function loadStatus(items) {
  let list = document.getElementById("table_list")
  while (list.hasChildNodes()) {
    list.removeChild(list.firstChild)
  }

  $(items).each(function () {
    let id = "table_" +this.table;
    if (document.getElementById(id) == null){
      $("#table_list").prepend(
        "<li>" + 
          "table id" + this.table + " : " + this.status +
        "</li>"
      )
    }
  })
}

function test_post() {
  //var commentText = document.getElementById("id_comment_input_text_" + id)
  //let itemTextElement = document.getElementById("id_comment_input_text_" + id)
  //let itemTextValue   = itemTextElement.value

  let xhr = new XMLHttpRequest()
  xhr.onreadystatechange = function() {
      if (xhr.readyState != 4) return
      
  }

  xhr.open("POST", "/data", true);
  xhr.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
  xhr.send("table_id=1" + "&status=occupied");
  //xhr.send("text=" + itemTextValue + "&id=" + id + "&csrfmiddlewaretoken=" + getCSRFToken());
}


