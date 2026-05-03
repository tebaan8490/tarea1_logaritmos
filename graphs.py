import pandas as pd
import matplotlib.pyplot as plt

df_const = pd.read_csv("data/resultados_construccion.csv")

fig, axes = plt.subplots(1, 2, figsize=(12, 5))

for dataset, ax in zip(["Random", "Europa"], axes):
    sub = df_const[df_const["dataset"] == dataset]
    for metodo, linea in [("NearestX", "-"), ("STR", "--")]:
        s = sub[sub["metodo"] == metodo]
        ax.plot(s["N"], s["tiempo_ms"],
                linestyle=linea, marker="o", label=metodo)
    ax.set_title(f"Tiempo de construcción — {dataset}")
    ax.set_xlabel("N")
    ax.set_ylabel("Tiempo (ms)")
    ax.set_xscale("log", base=2)
    ax.legend()
    ax.grid(True)

plt.tight_layout()
plt.savefig("data/grafico_construccion.png", dpi=150)
plt.show()

df = pd.read_csv("data/resultados_consultas.csv")
arboles = df["arbol"].unique()

fig, axes = plt.subplots(1, 2, figsize=(12, 5))

for nombre in arboles:
    sub = df[df["arbol"] == nombre]
    linea = "--" if "STR" in nombre else "-"
    color = "steelblue" if "Rand" in nombre else "tomato"
    axes[0].plot(sub["s"], sub["avg_ios"],
                 linestyle=linea, color=color,
                 marker="o", label=nombre)

axes[0].set_title("I/Os promedio por consulta")
axes[0].set_xlabel("s (lado del cuadrado)")
axes[0].set_ylabel("I/Os promedio")
axes[0].legend()
axes[0].grid(True)

for nombre in arboles:
    sub = df[df["arbol"] == nombre]
    linea = "--" if "STR" in nombre else "-"
    color = "steelblue" if "Rand" in nombre else "tomato"
    axes[1].errorbar(sub["s"], sub["avg_puntos"],
                     yerr=sub["std_puntos"],
                     linestyle=linea, color=color,
                     marker="o", label=nombre,
                     capsize=4)

axes[1].set_title("Puntos encontrados por consulta")
axes[1].set_xlabel("s (lado del cuadrado)")
axes[1].set_ylabel("Puntos promedio")
axes[1].legend()
axes[1].grid(True)

plt.tight_layout()
plt.savefig("data/grafico_consultas.png", dpi=150)
plt.show()