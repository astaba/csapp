// DOCUMENTATION: AJAX Data Handling for C Backends
// * 1. MULTIPART (FormData):
// - Browser sends: Content-Type: multipart/form-data; boundary=----WebKitFormBoundary...
// - Body contains: Headers for every single input field.
// - Result: Very difficult to parse in a simple C program (adder.c).
// * 2. URL-ENCODED (URLSearchParams):
// - Browser sends: Content-Type: application/x-www-form-urlencoded
// - Body contains: a=5&b=10
// - Result: Perfect for simple C string parsing (sscanf, strtok).

(function () {
    document.addEventListener('DOMContentLoaded', () => {

	const elResult = document.getElementById('result');
	let timeoutId;
	
	function fetchWithTimeout(url, options) {
	    if (options.timeout) {
		const controller = new AbortController();
		options.signal = controller.signal;
		timeoutId = setTimeout(() => controller.abort(), options.timeout);
	    }
	    return fetch(url, options);
	}

	function displayJsobj(jsobj) {
	    elResult.textContent = `${jsobj.data.result}`
	}

	document.getElementById('adder-form').addEventListener('submit', async (e) => {
	    e.preventDefault();

	    // // GET-Form + Ajax
	    // const payload = new URLSearchParams(new FormData(e.target)).toString();
	    // const url = `/cgi-bin/adder?${payload}`;
	    // const options = {
	    // 	method: 'GET',
	    // 	timeout: 2000,
	    // };

	    // UrlEncoded POST-Form + Ajax
	    const payload = new URLSearchParams(new FormData(e.target));
	    // With this payload the browser will force content-type to:
	    // application/x-www-form-urlencoded
	    const url = '/cgi-bin/adder';
	    const options = {
		method: 'POST',
		timeout: 2000,
		body: payload,
	    };

	    // // Multipart POST-Form + Ajax
	    // const payload = new FormData(e.target);
	    // // With this payload the browser will force content-type to:
	    // // multipart/form-data; boundary=----WebKitFormBoundaryxXxXXXxXXXxXXXXX
	    // const url = '/cgi-bin/adder';
	    // const options = {
	    // 	method: 'POST',
	    // 	timeout: 2000,
	    // 	body: payload,
	    // };

	    fetchWithTimeout(url, options)
		.then(response => {
		    const contentType = response.headers.get('content-type');
		    if (response.ok && contentType && contentType.includes('application/json')) {
			return response.json();
		    } else {
			throw new Error(`Unexpected response status ${response.status} (${response.statusText}) or content type.`);
		    }
		}).then(jsobj => {
		    console.log('Payload:', jsobj);
		    displayJsobj(jsobj);
		}).catch(error => {
		    elResult.textContent = error.message;
		    console.error('Ajax failure:', error);
		}).finally(() => {
		    if (timeoutId) {
			clearTimeout(timeoutId);
			timeoutId = undefined;  // Optional: reset for clarity
		    }
		});
	});
    });
})();
