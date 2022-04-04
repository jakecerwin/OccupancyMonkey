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
      refreshPage(xhr)
  }

  xhr.open("GET", "/get-status", true)
  xhr.send()
}


function loadPage(xhr) {
  if (xhr.status == 200) {
      let response = JSON.parse(xhr.responseText)
      loadStream(response)
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