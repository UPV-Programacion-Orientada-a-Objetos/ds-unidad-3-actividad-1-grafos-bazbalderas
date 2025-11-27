import streamlit as st
import neuro_net
import networkx as nx
import matplotlib.pyplot as plt
import tempfile
import os
import time

st.set_page_config(page_title="NeuroNet", layout="wide")

st.title("NeuroNet: Análisis y Visualización de Propagación en Redes Masivas")
st.markdown("### Sistema Híbrido C++ / Python con Matrices Dispersas (CSR)")

# Initialize session state
if 'engine' not in st.session_state:
    st.session_state.engine = None
if 'graph_loaded' not in st.session_state:
    st.session_state.graph_loaded = False

# Sidebar
st.sidebar.header("Configuración")
uploaded_file = st.sidebar.file_uploader("Cargar Dataset (Edge List)", type=["txt"])

# Add a sample dataset option for quick testing
use_sample = st.sidebar.checkbox("Usar Dataset de Prueba (Generado)")

if use_sample and not st.session_state.graph_loaded:
    # Generate a dummy file
    with tempfile.NamedTemporaryFile(delete=False, mode='w', suffix=".txt") as tmp_file:
        # Star graph centered at 0
        for i in range(1, 100):
            tmp_file.write(f"0 {i}\n")
            # Connect some leaves
            if i % 10 == 0:
                tmp_file.write(f"{i} {i+1}\n")
        tmp_path = tmp_file.name
    
    st.session_state.engine = neuro_net.NeuroNetEngine()
    st.session_state.engine.load_data(tmp_path)
    st.session_state.load_time = 0.001
    st.session_state.graph_loaded = True
    st.session_state.tmp_path = tmp_path
    st.sidebar.success("Dataset de prueba cargado.")

if uploaded_file is not None and not st.session_state.graph_loaded:
    # Save to temp file
    with tempfile.NamedTemporaryFile(delete=False, suffix=".txt") as tmp_file:
        tmp_file.write(uploaded_file.getvalue())
        tmp_path = tmp_file.name
    
    st.session_state.engine = neuro_net.NeuroNetEngine()
    
    start_time = time.time()
    st.session_state.engine.load_data(tmp_path)
    end_time = time.time()
    
    st.session_state.load_time = end_time - start_time
    st.session_state.graph_loaded = True
    st.session_state.tmp_path = tmp_path
    st.sidebar.success(f"Grafo cargado en {st.session_state.load_time:.4f}s")

if st.session_state.graph_loaded:
    engine = st.session_state.engine
    
    # Metrics
    col1, col2, col3 = st.columns(3)
    col1.metric("Nodos", engine.get_node_count())
    col2.metric("Aristas", engine.get_edge_count())
    col3.metric("Tiempo de Carga", f"{st.session_state.load_time:.4f}s")
    
    st.divider()
    
    # Analysis
    st.subheader("Análisis Topológico")
    if st.button("Identificar Nodo Crítico (Mayor Grado)"):
        node_id = engine.get_max_degree_node()
        st.info(f"El nodo con mayor grado es: **{node_id}**")
        
    st.divider()
    
    # Simulation
    st.subheader("Simulación de Recorrido (BFS)")
    col_sim1, col_sim2 = st.columns(2)
    start_node = col_sim1.number_input("Nodo de Inicio", min_value=0, value=0)
    depth = col_sim2.number_input("Profundidad Máxima", min_value=1, max_value=5, value=2)
    
    if st.button("Ejecutar BFS"):
        start_bfs = time.time()
        visited_nodes = engine.bfs(start_node, depth)
        bfs_time = time.time() - start_bfs
        
        st.success(f"BFS completado en {bfs_time:.6f}s. Nodos encontrados: {len(visited_nodes)}")
        
        # Visualization
        if len(visited_nodes) > 0:
            if len(visited_nodes) < 500:
                st.write("Generando visualización...")
                G = nx.Graph()
                
                visited_set = set(visited_nodes)
                
                # Reconstruct subgraph edges
                for u in visited_nodes:
                    neighbors = engine.get_neighbors(u)
                    for v in neighbors:
                        if v in visited_set:
                            G.add_edge(u, v)
                            
                if G.number_of_nodes() == 0:
                     G.add_node(start_node)

                fig, ax = plt.subplots(figsize=(10, 8))
                # Use shell layout or spring
                pos = nx.spring_layout(G, seed=42)
                
                nx.draw_networkx_nodes(G, pos, node_size=300, node_color='skyblue', alpha=0.8)
                nx.draw_networkx_edges(G, pos, alpha=0.5)
                nx.draw_networkx_labels(G, pos, font_size=10)
                
                # Highlight start node
                if start_node in G:
                    nx.draw_networkx_nodes(G, pos, nodelist=[start_node], node_color='red', node_size=500)
                
                st.pyplot(fig)
            else:
                st.warning("El subgrafo es demasiado grande para visualizar (>500 nodos).")
        else:
            st.warning("No se encontraron nodos (verifique el nodo de inicio).")

if st.sidebar.button("Reiniciar"):
    st.session_state.graph_loaded = False
    st.session_state.engine = None
    st.rerun()
