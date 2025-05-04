# HTTP Error Codes Documentation

<br>

---

<br>

<details>
<summary>French Version</summary>

Ce document présente une classification des codes d'erreur HTTP que peut renvoyer un serveur. Chaque section détaille une catégorie de réponses HTTP avec une description concise des codes spécifiques.

---

## 🟦 1xx - Réponses informatives
Réponses temporaires indiquant que le client doit continuer sa requête.

- **100 Continue** : Attente de la suite de la requête.

---

## 🟩 2xx - Réponses de succès
Le serveur a reçu, compris et accepté la requête.

- **200 OK** : Requête réussie.
- **201 Created** : Ressource créée.
- **204 No Content** : Pas de contenu à envoyer.

---

## 🟨 3xx - Redirections
Le client doit effectuer une action supplémentaire pour compléter la requête.

- **301 Moved Permanently** : Ressource déplacée de façon permanente.
- **302 Found** : Ressource temporairement déplacée.
- **303 See Other** : Voir autre URI.
- **307 Temporary Redirect** : Redirection temporaire.

---

## 🟥 4xx - Erreurs du client
Erreur de syntaxe ou requête impossible à satisfaire.

- **400 Bad Request** : Syntaxe de la requête erronée.
- **401 Unauthorized** : Authentification requise.
- **403 Forbidden** : Accès refusé.
- **404 Not Found** : Ressource non trouvée.
- **405 Method Not Allowed** : Méthode non autorisée.
- **413 Payload Too Large** : Charge trop lourde.
- **414 URI Too Long** : URI trop longue.
- **415 Unsupported Media Type** : Type média non supporté.
- **499 Client Closed Request** : Client a fermé la connexion.

---

## 🟧 5xx - Erreurs du serveur
Erreur du serveur en traitant une requête correcte.

- **500 Internal Server Error** : Erreur interne.
- **501 Not Implemented** : Fonctionnalité non implémentée.
- **502 Bad Gateway** : Mauvaise réponse d'un serveur intermédiaire.
- **503 Service Unavailable** : Service non disponible.
- **504 Gateway Timeout** : Délai d'attente dépassé pour un serveur intermédiaire.
- **505 HTTP Version Not Supported** : Version HTTP non supportée.

---

</details>

<br>

---

<br>

<details>
<summary>English Version</summary>

This document provides a classification of HTTP error codes that a server can return. Each section details a category of HTTP responses with a concise description of specific codes.

---

## 🟦 1xx - Informational Responses
Temporary responses indicating that the client should continue with the request.

- **100 Continue**: Waiting for the continuation of the request.

---

## 🟩 2xx - Success Responses
The server has successfully received, understood, and accepted the request.

- **200 OK**: Request succeeded.
- **201 Created**: Resource created.
- **204 No Content**: No content to send.

---

## 🟨 3xx - Redirections
The client must take additional action to complete the request.

- **301 Moved Permanently**: Resource permanently moved.
- **302 Found**: Resource temporarily moved.
- **303 See Other**: See another URI.
- **307 Temporary Redirect**: Temporary redirection.

---

## 🟥 4xx - Client Errors
Syntax error or request cannot be fulfilled.

- **400 Bad Request**: Malformed request syntax.
- **401 Unauthorized**: Authentication required.
- **403 Forbidden**: Access denied.
- **404 Not Found**: Resource not found.
- **405 Method Not Allowed**: Method not allowed.
- **413 Payload Too Large**: Payload too large.
- **414 URI Too Long**: URI too long.
- **415 Unsupported Media Type**: Unsupported media type.
- **499 Client Closed Request**: Client closed the connection.

---

## 🟧 5xx - Server Errors
Server error while processing a valid request.

- **500 Internal Server Error**: Internal error.
- **501 Not Implemented**: Functionality not implemented.
- **502 Bad Gateway**: Bad response from an intermediary server.
- **503 Service Unavailable**: Service unavailable.
- **504 Gateway Timeout**: Timeout from an intermediary server.
- **505 HTTP Version Not Supported**: HTTP version not supported.

---

</details>