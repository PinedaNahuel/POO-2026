#ifndef TASK_H
#define TASK_H

#include <QString>
#include <QJsonObject>

class Task
{
public:
    int id;
    QString titulo;
    QString materia;
    QString descripcion;
    QString estado;
    QString prioridad;
    QString fechaEntrega;

    Task()
    {
        id = 0;
    }

    QJsonObject toJson() const
    {
        QJsonObject obj;
        obj["id"] = id;
        obj["titulo"] = titulo;
        obj["materia"] = materia;
        obj["descripcion"] = descripcion;
        obj["estado"] = estado;
        obj["prioridad"] = prioridad;
        obj["fechaEntrega"] = fechaEntrega;
        return obj;
    }

    static Task fromJson(const QJsonObject &obj)
    {
        Task task;
        task.id = obj["id"].toInt();
        task.titulo = obj["titulo"].toString();
        task.materia = obj["materia"].toString();
        task.descripcion = obj["descripcion"].toString();
        task.estado = obj["estado"].toString();
        task.prioridad = obj["prioridad"].toString();
        task.fechaEntrega = obj["fechaEntrega"].toString();
        return task;
    }
};

#endif // TASK_H
