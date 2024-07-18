# Documentation des Codes d'Erreurs HTTP

Ce document présente une classification des codes d'erreur HTTP que peut renvoyer un serveur. Chaque section ci-dessous détaille une catégorie de réponses HTTP avec un tableau récapitulatif des codes spécifiques et leurs significations.

## 1xx - Réponses informatives
Réponses temporaires indiquant que le client doit continuer sa requête.

| Code + Message       | Description         |
|----------------------|---------------------|
| 100 Continue         | Attente de la suite de la requête. |

## 2xx - Réponses de succès
Le serveur a reçu, compris et accepté la requête.

| Code + Message       | Description         |
|----------------------|---------------------|
| 200 OK               | Requête réussie.    |
| 201 Created          | Ressource créée.    |
| 204 No Content       | Pas de contenu à envoyer. |

## 3xx - Redirections
Le client doit effectuer une action supplémentaire pour compléter la requête.

| Code + Message       | Description                  |
|----------------------|------------------------------|
| 301 Moved Permanently| Ressource déplacée de façon permanente. |
| 302 Found            | Ressource temporairement déplacée. |
| 303 See Other        | Voir autre URI.              |
| 307 Temporary Redirect| Redirection temporaire.     |

## 4xx - Erreurs du client
Erreur de syntaxe ou requête impossible à satisfaire.

| Code + Message       | Description                  |
|----------------------|------------------------------|
| 400 Bad Request      | Syntaxe de la requête erronée. |
| 401 Unauthorized     | Authentification requise.    |
| 403 Forbidden        | Accès refusé.                |
| 404 Not Found        | Ressource non trouvée.       |
| 405 Method Not Allowed| Méthode non autorisée.      |
| 413 Payload Too Large| Charge trop lourde.          |
| 414 URI Too Long     | URI trop longue.             |
| 415 Unsupported Media Type| Type média non supporté.|
| 499 Client Closed Request| Client a fermé la connexion.|

## 5xx - Erreurs du serveur
Erreur du serveur en traitant une requête correcte.

| Code + Message       | Description                  |
|----------------------|------------------------------|
| 500 Internal Server Error| Erreur interne.         |
| 501 Not Implemented  | Fonctionnalité non implémentée. |
| 502 Bad Gateway      | Mauvaise réponse d'un serveur intermédiaire. |
| 503 Service Unavailable| Service non disponible.   |
| 504 Gateway Timeout  | Délai d'attente dépassé pour un serveur intermédiaire. |
| 505 HTTP Version Not Supported| Version HTTP non supportée. |
