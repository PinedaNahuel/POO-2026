# Endpoint de salud esperado

La aplicacion acepta un endpoint HTTP que responda JSON con este formato base:

```json
{
  "status": "ok",
  "uptime_seconds": 345678,
  "cpu_percent": 41.2,
  "memory_percent": 63.5,
  "disk_percent": 71.8,
  "latency_ms": 126,
  "temperature_c": 54.7,
  "checked_at": "2026-03-30T16:42:10Z",
  "events": [
    { "time": "16:35", "message": "Backup nocturno finalizado" },
    { "time": "16:39", "message": "Latencia dentro del rango esperado" }
  ]
}
```

Tambien se admite una variante con objeto `metrics`:

```json
{
  "status": "warning",
  "checked_at": "2026-03-30T16:42:10Z",
  "metrics": {
    "uptime_seconds": 345678,
    "cpu_percent": 87.4,
    "memory_percent": 74.9,
    "disk_percent": 81.1,
    "latency_ms": 240
  },
  "events": ["CPU por encima del umbral de seguridad"]
}
```

Valores de `status` reconocidos:

- `ok`, `healthy`, `up`
- `alert`, `warning`, `degraded`
- `down`, `critical`, `offline`, `error`

Mientras no tengas tu VPS conectado, podes dejar `demo://vps-health` en la URL para usar el origen de datos interno de demostracion.