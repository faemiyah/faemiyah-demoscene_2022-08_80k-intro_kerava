#ifndef INTRO_WORLD_HPP
#define INTRO_WORLD_HPP

/// Number of slices in a glyph.
constexpr unsigned WORLD_GLYPH_SLICE_COUNT = 8;

/// Glyph mesh array type.
using GlyphMeshArray = vgl::array<const vgl::Mesh*, WORLD_GLYPH_SLICE_COUNT>;

/// Create world position.
///
/// \param px X position (x10).
/// \param py Y position (x10).
/// \param pz Z position (x10).
/// \param rx Rotation (0-255).
/// \param ry Rotation (0-255).
/// \param rz Rotation (0-255).
/// \return Rotation matrix.
VGL_MATH_CONSTEXPR static vgl::mat4 world_position(int px, int py, int pz, int rx, int ry, int rz)
{
    const float ROT_MUL = static_cast<float>(M_PI * 2.0 / 256.0);
    vgl::vec3 pos(static_cast<float>(px), static_cast<float>(py), static_cast<float>(pz));
    float frx = static_cast<float>(rx) * ROT_MUL;
    float fry = static_cast<float>(ry) * ROT_MUL;
    float frz = static_cast<float>(rz) * ROT_MUL;
    return vgl::mat4::rotation_euler(frx, fry, frz, pos * 0.1f);
}

/// Create world position.
///
/// \param px X position (x10).
/// \param py Y position (x10).
/// \param pz Z position (x10).
/// \return Rotation matrix.
VGL_MATH_CONSTEXPR static vgl::mat4 world_position(int px, int py, int pz)
{
    return world_position(px, py, pz, 0, 0, 0);
}

/// Sent text drawing command to the queue.
///
/// \param queue Queue to send the commands.
/// \param pen_pos Pen position.
/// \param transform Transform for the text.
/// \param font Font to use.
/// \param glyph Glyph to use.
/// \param font_size Font size.
/// \param text Text to draw.
void draw_text(vgl::RenderQueue& queue, const vgl::vec3& pen_pos, const vgl::mat4& transform, const vgl::Font& font,
        const GlyphMeshArray& glyph, const vgl::vec2& font_size, vgl::string_view text)
{
    vgl::vec3 pos = pen_pos;
    float line = 0.0f;
    for(auto& vv : text)
    {
        // Line change.
        if(static_cast<int>(vv) == '\n')
        {
            line -= font_size.y();
            pos = pen_pos + vgl::vec3(0.0f, line, 0.0f);
            continue;
        }

        // Rendering a glyph.
        const vgl::Character& cc = font.getCharacter(static_cast<unsigned>(vv));
        vgl::vec2 quad_offset = cc.getQuadOffset() * font_size;
        vgl::vec2 quad_size = cc.getQuadSize() * font_size;
        vgl::vec4 quad(quad_offset.x(), quad_offset.y(), quad_size.x(), quad_size.y());

        {
            bool uniforms_sent = false;
            for(const auto& mesh : glyph)
            {
                if(mesh)
                {
                    queue.push(*mesh, transform);
                    if(!uniforms_sent)
                    {
                        queue.push(vgl::UniformSemantic::GLYPH_TEXTURE, cc.getTexture());
                        queue.push(vgl::UniformSemantic::GLYPH_POSITION, pos);
                        queue.push(vgl::UniformSemantic::GLYPH_QUAD, quad);
                        uniforms_sent = true;
                    }
                }
            }
        }

        // Move the pen forward.
        pos += vgl::vec3(font_size.x() * cc.getAdvanceX(), 0.0f, 0.0f);
    }
}

/// Database for intro objects for constructing a 'world' of sorts.
///
/// There may be multiple worlds.
///
/// Rendering a world pushes the static objects within for rendering.
/// User may hook modifier functions to alter the parameters for given objects.
///
/// Worlds are divided and sorted on the z axis, everything touching the visibility box is rendered.
/// Visibility box is defined as starting from a Z coordinate and extending towards the negative direction.
class IntroWorld
{
public:
    /// Callback for rendering a mesh.
    ///
    /// \param queue Queue to push rendered meshes to.
    /// \param program Program to render the mesh with.
    /// \param mesh Mesh to be rendered.
    /// \param transform Transformation to render with.
    using EntityCallback = void (*)(vgl::RenderQueue& queue, const vgl::GlslProgram& program, const vgl::Mesh& mesh,
            const vgl::mat4& transform);

private:
    /// Class for holding a mesh and a position.
    class Entity
    {
    private:
        /// Program to use for rendering.
        const vgl:: GlslProgram& m_program;

        /// Mesh to be rendered.
        const vgl::Mesh* m_mesh = nullptr;

        /// Font to be rendered.
        const vgl::Font* m_font = nullptr;

        /// Glyph mesh array.
        const GlyphMeshArray* m_glyph = nullptr;

        /// Font size (if font is set).
        vgl::vec2 m_font_size;

        /// Pen position (if font is set).
        vgl::vec3 m_pen_pos;

        /// Text string (if font is set).
        vgl::string m_text;

        /// Transformation of the mesh.
        vgl::mat4 m_transform;

        /// Callback to run when rendering the entity.
        /// Null for static objects.
        EntityCallback m_callback = nullptr;

        /// Bounding box (transformed).
        /// Cached before sorting.
        vgl::BoundingBox m_transformed_bounding_box;

    public:
        /// Constructor.
        ///
        /// \param program Program to render with.
        /// \param mesh Mesh to render.
        /// \param transform Transformation to use.
        /// \param callback Callback to use.
        explicit Entity(const vgl::GlslProgram& program, const vgl::Mesh& mesh, const vgl::mat4& transform,
                EntityCallback callback) noexcept :
            m_program(program),
            m_mesh(&mesh),
            m_transform(transform),
            m_callback(callback)
        {
            cacheBoundingBox(*m_mesh);
        }

        /// Constructor.
        ///
        /// \param program Program to render with.
        /// \param font Font to use.
        /// \param glyph Glyph mesh to use.
        /// \param pen_pos Pen position.
        /// \param pen_dir Pen direction.
        /// \param pen_up Pen up vector.
        /// \param text Text to print.
        /// \param transform Transformation to use.
        /// \param callback Callback to use.
        explicit Entity(const vgl::GlslProgram& program, const vgl::Font& font, const GlyphMeshArray& glyph,
                const vgl::vec2& font_size, const vgl::vec3& pen_pos, vgl::string_view text, const vgl::mat4& transform,
                EntityCallback callback) noexcept :
            m_program(program),
            m_font(&font),
            m_glyph(&glyph),
            m_font_size(font_size),
            m_pen_pos(pen_pos),
            m_text(text),
            m_transform(transform),
            m_callback(callback)
        {
            cacheBoundingBox(*((*m_glyph)[0]));
        }

    protected:
        /// Cache the bounding box based on given transformation.
        constexpr void cacheBoundingBox(const vgl::Mesh& mesh) noexcept
        {
            m_transformed_bounding_box = mesh.getBoundingBox().transform(m_transform);
        }

    public:
        /// Accessor.
        ///
        /// \return Font.
        constexpr const vgl::Font* getFont() const noexcept
        {
            return m_font;
        }

        /// Accessor.
        ///
        /// \return Glyph.
        constexpr const GlyphMeshArray* getGlyph() const noexcept
        {
            return m_glyph;
        }

        /// Accessor.
        ///
        /// \return Mesh.
        constexpr const vgl::Mesh* getMesh() const noexcept
        {
            return m_mesh;
        }

        /// Accessor.
        ///
        /// \return transform.
        constexpr const vgl::mat4& getTransform() const noexcept
        {
            return m_transform;
        }

        /// Test if the bounding box collides with another bounding box.
        ///
        /// \param op Bounding box to test with.
        constexpr bool collides(const vgl::BoundingBox& op) const noexcept
        {
            return m_transformed_bounding_box.collides(op);
        }

        /// Test if the bounding box collides with given z range.
        ///
        /// \param zmin Smaller Z value.
        /// \param zmax Larger Z value.
        /// \return True if collides, false otherwise.
        constexpr bool collidesZ(float zmin, float zmax) const noexcept
        {
            return m_transformed_bounding_box.collidesZ(zmin, zmax);
        }

        /// Calculate the center point, taking into account the transformation.
        ///
        /// \return Center point.
        constexpr vgl::vec3 getCenter() const noexcept
        {
            return m_transformed_bounding_box.getCenter();
        }

        /// Render the entity.
        ///
        /// \param queue Queue to push the entity to.
        void render(vgl::RenderQueue& queue) const
        {
            if(m_callback)
            {
                m_callback(queue, m_program, *m_mesh, m_transform);
            }
            else
            {
                if(m_font)
                {
                    queue.push(m_program);
                    draw_text(queue, m_pen_pos, m_transform, *m_font, *m_glyph, m_font_size, m_text);
                }
                else
                {
                    queue.push(m_program);
                    queue.push(*m_mesh, m_transform);
                }
            }
        }

    public:
        /// Comparison function.
        ///
        /// \param lhs Left-hand-side operand
        /// \param rhs Right-hand-side operand.
        /// \return -1 if lhs is 'less' than rhs, 1 if lhs is 'greater' than rhs. Zero if equal.
        constexpr static int qsort_compare(const void* lhs, const void* rhs)
        {
            const Entity* ea = static_cast<const Entity*>(lhs);
            const Entity* eb = static_cast<const Entity*>(rhs);

            vgl::vec3 ca = ea->getCenter();
            vgl::vec3 cb = eb->getCenter();

            if(ca.z() < cb.z())
            {
                return -1;
            }
            else if(ca.z() > cb.z())
            {
                return 1;
            }
            return 0;
        }
    };

private:
    /// List of entities.
    vgl::vector<Entity> m_entities;

    /// Z extents to use for rendering.
    /// 0: Positive extent.
    /// 1: Negative extent.
    /// 2: Search range.
    vgl::vec3 m_z_extents;

public:
    /// Default constructor.
    explicit constexpr IntroWorld() = default;

public:
    /// Add a new entity.
    ///
    /// \param program Program to render with.
    /// \param mesh Mesh to render.
    /// \param transform Transformation to use.
    /// \param callback Callback to use.
    void addEntity(const vgl::GlslProgram& program, const vgl::Mesh& mesh, const vgl::mat4& transform,
            EntityCallback callback = nullptr)
    {
        m_entities.emplace_back(program, mesh, transform, callback);
    }
    /// Add a new entity.
    ///
    /// \param program Program to render with.
    /// \param mesh Mesh to render.
    /// \param px X coordinate.
    /// \param py Y coordinate.
    /// \param pz Z coordinate.
    /// \param rx X rotation.
    /// \param ry Y rotation.
    /// \param rz Z rotation.
    void addEntity(const vgl::GlslProgram& program, const vgl::Mesh& mesh, const vgl::vec3& pos, float rx, float ry, float rz)
    {
        addEntity(program, mesh, vgl::mat4::rotation_euler(rx, ry, rz, pos));
    }
    /// Add a new entity.
    ///
    /// \param program Program to render with.
    /// \param mesh Mesh to render.
    /// \param px X coordinate (x10).
    /// \param py Y coordinate (x10).
    /// \param pz Z coordinate (x10).
    /// \param rx X rotation (0-255).
    /// \param ry Y rotation (0-255).
    /// \param rz Z rotation (0-255).
    void addEntity(const vgl::GlslProgram& program, const vgl::Mesh& mesh, int px, int py, int pz, int rx, int ry, int rz)
    {
        addEntity(program, mesh, world_position(px, py, pz, rx, ry, rz));
    }
    /// Add a new entity.
    ///
    /// \param program Program to render with.
    /// \param mesh Mesh to render.
    /// \param px X coordinate (x10).
    /// \param py Y coordinate (x10).
    /// \param pz Z coordinate (x10).
    /// \param callback Callback to use.
    void addEntity(const vgl::GlslProgram& program, const vgl::Mesh& mesh, int px, int py, int pz, EntityCallback callback)
    {
        addEntity(program, mesh, world_position(px, py, pz), callback);
    }
    /// Add a new entity.
    ///
    /// \param program Program to render with.
    /// \param mesh Mesh to render.
    /// \param px X coordinate (x10).
    /// \param py Y coordinate (x10).
    /// \param pz Z coordinate (x10).
    void addEntity(const vgl::GlslProgram& program, const vgl::Mesh& mesh, int px, int py, int pz)
    {
        addEntity(program, mesh, px, py, pz, nullptr);
    }

    /// Add a new entity.
    ///
    /// \param program Program to render with.
    /// \param font Font to use.
    /// \param mesh Glyph mesh to use.
    /// \param font_size Font size.
    /// \param pen_pos Pen position.
    /// \param text Text to print.
    /// \param transform Transformation to use.
    /// \param callback Callback to use.
    void addEntity(const vgl::GlslProgram& program, const vgl::Font& font, const GlyphMeshArray& glyph,
            const vgl::vec2& font_size, const vgl::vec3& pen_pos, vgl::string_view text, const vgl::mat4& transform,
            EntityCallback callback = nullptr)
    {
        m_entities.emplace_back(program, font, glyph, font_size, pen_pos, text, transform, callback);
    }

    /// Try to add entity, do not add if it conflicts with bounding box of an existing entity.
    ///
    /// \param program Program to render with.
    /// \param mesh Mesh to render.
    /// \param px X coordinate.
    /// \param py Y coordinate.
    /// \param pz Z coordinate.
    /// \param rx X rotation.
    /// \param ry Y rotation.
    /// \param rz Z rotation.
    bool tryAddEntity(const vgl::GlslProgram& program, const vgl::Mesh& mesh, const vgl::vec3& pos, float rx, float ry, float rz)
    {
        vgl::mat4 trns = vgl::mat4::rotation_euler(rx, ry, rz, pos);
        vgl::BoundingBox trns_box = mesh.getBoundingBox().transform(trns);
        for(auto& vv : m_entities)
        {
            if(vv.collides(trns_box))
            {
                return false;
            }
        }
        addEntity(program, mesh, trns);
        return true;
    }

    /// Sort the entities.
    ///
    /// Should be called after everything has been done.
    void sort()
    {
        dnload_qsort(m_entities.data(), m_entities.size(), sizeof(Entity), Entity::qsort_compare);
    }

    /// Render the world.
    ///
    /// Draws all entities from given z coordinate towards the negative.
    ///
    /// \param queue Render queue to use.
    /// \param zmax Maximum Z coordinate - where to start from.
    /// \param zmin Minimum Z coordinate - where to extend to.
    /// \param tolerance Tolerance in coordinates to keep looking for.
    void render(vgl::RenderQueue& queue, float zmax, float zmin, float tolerance)
    {
        // First, find the center position.
        unsigned idx = findClosestEntityIndex(zmax);

        // Search towards negative.
        for(int ii = static_cast<int>(idx); (ii >= 0); --ii)
        {
            Entity& entity = m_entities[ii];

            // Colliding entities are rendered immediately.
            if(entity.collidesZ(zmin, zmax))
            {
                entity.render(queue);
                continue;
            }

            // Otherwise check if we've gone too far.
            if(entity.getCenter().z() < (zmin - tolerance))
            {
                break;
            }
        }

        // Search towards positive.
        for(unsigned ii = idx; (ii < m_entities.size()); ++ii)
        {
            Entity& entity = m_entities[ii];

            // Colliding entities are rendered immediately.
            if(entity.collidesZ(zmin, zmax))
            {
                entity.render(queue);
                continue;
            }

            // Otherwise check if we've gone too far.
            if(entity.getCenter().z() > (zmax + tolerance))
            {
                break;
            }
        }
    }
    /// Render the world.
    ///
    /// Z rendering extents are ordered as follows.
    /// 0: Positive extent (must be > 0).
    /// 1: Negative extent (must be < 0).
    /// 2: Search range (must be > 0).
    ///
    /// Rendered objects are selected based on position and z extent search.
    ///
    /// \param queue Render queue to use.
    /// \param pos Camera position.
    /// \param z_extents Z extents to use for rendering.
    void render(vgl::RenderQueue& queue, const vgl::vec3& pos, const vgl::vec3& z_extents)
    {
        VGL_ASSERT(z_extents.x() > 0.0f);
        VGL_ASSERT(z_extents.y() < 0.0f);
        VGL_ASSERT(z_extents.z() > 0.0f);
        render(queue, pos.z() + z_extents.x(), pos.z() + z_extents.y(), z_extents.z());
    }

protected:
    /// Find closest index to given Z coordinate value.
    ///
    /// \param op Z coordinate value.
    /// \return Index in the entioty array with an entity with closest center position.
    unsigned findClosestEntityIndex(float op)
    {
        unsigned lo = 0;
        unsigned hi = m_entities.size() - 1;

        while(lo != hi)
        {
            // If already too close together, select one or the other.
            if((hi - lo) <= 1)
            {
                float z1 = m_entities[lo].getCenter().z();
                float z2 = m_entities[hi].getCenter().z();
                if(abs(z1 - op) < abs(z2 - op))
                {
                    return lo;
                }
                return hi;
            }

            unsigned mid = (lo + hi) / 2;
            float cz = m_entities[mid].getCenter().z();
            if(cz > op)
            {
                hi = mid;
            }
            else if(cz < op)
            {
                lo = mid;
            }
            else
            {
                return mid;
            }
        }

        return lo;
    }
};

#endif
