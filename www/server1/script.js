// Fonction pour afficher ou masquer le menu de navigation
function toggleNavigation() {
    var navigation = document.getElementById("navigation");
    navigation.classList.toggle("show");
}

// Fonction pour changer le light/dark mode
const toggleButton = document.querySelector('.color-toggle-button');
const body = document.querySelector('body');
toggleButton.addEventListener('click', () => {
	body.dataset.theme = body.dataset.theme === 'light' ? 'dark' : 'light';
});

// Fonction pour lister les fichiers
function listFiles() {
    var fileListElement = document.getElementById('fileList');
    if (fileListElement.style.display === 'none' || fileListElement.style.display === '') {
        fileListElement.style.display = 'block';
        var xhr = new XMLHttpRequest();
        xhr.open('GET', '../cgi-bin/list_file.php', true);
        xhr.onload = function() {
            if (this.status == 200) {
                fileListElement.innerHTML = this.responseText;
            }
        };
        xhr.send();
    } else {
        fileListElement.style.display = 'none';
    }
}

// Fonction pour obtenir le prix du Bitcoin
function toggleBitcoinPrice() {
    var bitcoinPriceElement = document.getElementById('bitcoinPrice');
    if (bitcoinPriceElement.style.display === 'none' || bitcoinPriceElement.style.display === '') {
        bitcoinPriceElement.style.display = 'block';
        var xhr = new XMLHttpRequest();
        xhr.open('GET', '../cgi-bin/bitcoin_price.sh', true);
        xhr.onload = function() {
            if (this.status == 200) {
                bitcoinPriceElement.innerHTML = this.responseText;
            }
        };
        xhr.send();
    } else {
        bitcoinPriceElement.style.display = 'none';
    }
}

function toggleViewCookie() {
	var cookieElement = document.getElementById('cookieElement');
	if (cookieElement.style.display === 'none' || cookieElement.style.display === '') {
		cookieElement.style.display = 'block';
		var xhr = new XMLHttpRequest();
		xhr.open('GET', '../cgi-bin/cookie.sh', true);
		xhr.onload = function() {
			if (this.status == 200) {
				cookieElement.innerHTML = this.responseText;
			}
		};
		xhr.send();
	} else {
		cookieElement.style.display = 'none';
	}
}
