const SUBJECT_NAMES = [
    "C++ Programming",
    "Data Structures",
    "Database Systems",
    "Operating Systems",
    "Computer Networks"
];

let allStudentsCache = [];

// Initialize application on DOM load
document.addEventListener('DOMContentLoaded', () => {
    setupNavigation();
    fetchDashboardStats();
    fetchStudents();
    calcPreview();
});

// ==========================================================================
// 1. Navigation & Section Switching
// ==========================================================================
function setupNavigation() {
    const navItems = document.querySelectorAll('.sidebar-menu .nav-item');
    navItems.forEach(item => {
        item.addEventListener('click', (e) => {
            e.preventDefault();
            const section = item.getAttribute('data-section');
            switchSection(section);
        });
    });
}

function switchSection(sectionId) {
    document.querySelectorAll('.view-section').forEach(sec => sec.classList.remove('active'));
    document.querySelectorAll('.sidebar-menu .nav-item').forEach(nav => nav.classList.remove('active'));

    const targetSec = document.getElementById(`section-${sectionId}`);
    const targetNav = document.querySelector(`.sidebar-menu .nav-item[data-section="${sectionId}"]`);

    if (targetSec) targetSec.classList.add('active');
    if (targetNav) targetNav.classList.add('active');

    // Trigger specific views
    if (sectionId === 'dashboard') {
        fetchDashboardStats();
        fetchStudents();
    } else if (sectionId === 'students') {
        fetchStudents();
    } else if (sectionId === 'merit-list') {
        renderMeritList(allStudentsCache);
    }
}

// ==========================================================================
// 2. Fetch Dashboard Statistics
// ==========================================================================
async function fetchDashboardStats() {
    try {
        const response = await fetch('/api/dashboard');
        if (!response.ok) throw new Error('Failed to load dashboard metrics');
        const data = await response.json();

        document.getElementById('stat-total').textContent = data.total_students;
        document.getElementById('stat-passed').textContent = data.passed_students;
        document.getElementById('stat-failed').textContent = data.failed_students;
        document.getElementById('stat-pass-rate').textContent = `${data.pass_percentage.toFixed(1)}%`;
        document.getElementById('stat-class-avg').textContent = `${data.class_average.toFixed(1)}%`;

        if (data.total_students > 0 && data.topper_name) {
            document.getElementById('stat-topper-name').textContent = data.topper_name;
            document.getElementById('stat-topper-roll').textContent = data.topper_roll;
            document.getElementById('stat-topper-perc').textContent = `${data.topper_percentage.toFixed(2)}%`;
        } else {
            document.getElementById('stat-topper-name').textContent = 'No records yet';
            document.getElementById('stat-topper-roll').textContent = '-';
            document.getElementById('stat-topper-perc').textContent = '0.00%';
        }
    } catch (err) {
        console.error('Error fetching dashboard stats:', err);
    }
}

// ==========================================================================
// 3. Fetch & Render Students
// ==========================================================================
async function fetchStudents(searchQuery = '') {
    try {
        let url = '/api/students';
        if (searchQuery.trim()) {
            url += `?search=${encodeURIComponent(searchQuery.trim())}`;
        }
        const response = await fetch(url);
        if (!response.ok) throw new Error('Failed to fetch students');
        const students = await response.json();

        allStudentsCache = students;
        renderMainTable(students);
        renderRecentPreview(students);
        renderMeritList(students);
    } catch (err) {
        console.error('Error fetching students:', err);
        showToast('Error loading student records from server', 'error');
    }
}

function getGradeBadgeClass(grade) {
    switch (grade) {
        case 'A+': return 'grade-aplus';
        case 'A':  return 'grade-a';
        case 'B':  return 'grade-b';
        case 'C':  return 'grade-c';
        case 'D':  return 'grade-d';
        default:   return 'grade-f';
    }
}

function renderMainTable(students) {
    const tbody = document.getElementById('students-tbody');
    const countLabel = document.getElementById('record-count-label');

    if (!students || students.length === 0) {
        tbody.innerHTML = `<tr><td colspan="10" class="text-center" style="padding: 2.5rem; color: var(--text-muted);">
            <i class="fa-solid fa-folder-open" style="font-size: 2rem; margin-bottom: 0.5rem; display: block;"></i>
            No student records found. Click "Add New Student" to get started!
        </td></tr>`;
        countLabel.textContent = 'Showing 0 students';
        return;
    }

    countLabel.textContent = `Showing ${students.length} student record${students.length > 1 ? 's' : ''}`;

    let html = '';
    students.forEach(s => {
        const gradeClass = getGradeBadgeClass(s.grade);
        const statusClass = s.status === 'Pass' ? 'status-pass' : 'status-fail';
        const statusIcon = s.status === 'Pass' ? '<i class="fa-solid fa-circle-check"></i>' : '<i class="fa-solid fa-circle-xmark"></i>';

        html += `
            <tr>
                <td><strong>#${s.roll_number}</strong></td>
                <td><span style="font-family: monospace; font-size: 0.85rem; color: var(--text-muted);">${s.student_id}</span></td>
                <td><strong>${escapeHtml(s.full_name)}</strong> <small style="color: var(--text-muted);">(${s.age} yrs)</small></td>
                <td>${escapeHtml(s.course)} (Sem ${s.semester})</td>
                <td>${escapeHtml(s.phone_number)}</td>
                <td><strong>${s.total_marks.toFixed(1)}</strong> / 500</td>
                <td>${s.percentage.toFixed(2)}%</td>
                <td><span class="badge-grade ${gradeClass}">${s.grade}</span></td>
                <td><span class="badge-status ${statusClass}">${statusIcon} ${s.status}</span></td>
                <td class="text-center">
                    <div class="action-buttons">
                        <button class="btn-action btn-action-view" title="View Official Marksheet" onclick="openMarksheetModal(${s.roll_number})">
                            <i class="fa-solid fa-file-invoice"></i>
                        </button>
                        <button class="btn-action btn-action-edit" title="Edit Student" onclick="openEditModal(${s.roll_number})">
                            <i class="fa-solid fa-pen"></i>
                        </button>
                        <button class="btn-action btn-action-delete" title="Delete Student" onclick="confirmDeleteStudent(${s.roll_number}, '${escapeHtml(s.full_name)}')">
                            <i class="fa-solid fa-trash-can"></i>
                        </button>
                    </div>
                </td>
            </tr>
        `;
    });
    tbody.innerHTML = html;
}

function renderRecentPreview(students) {
    const tbody = document.getElementById('dashboard-recent-tbody');
    if (!students || students.length === 0) {
        tbody.innerHTML = `<tr><td colspan="9" class="text-center" style="padding: 1.5rem; color: var(--text-muted);">No student records available yet.</td></tr>`;
        return;
    }

    const previewList = students.slice(0, 5); // top 5
    let html = '';
    previewList.forEach(s => {
        const gradeClass = getGradeBadgeClass(s.grade);
        const statusClass = s.status === 'Pass' ? 'status-pass' : 'status-fail';
        html += `
            <tr>
                <td><strong>#${s.roll_number}</strong></td>
                <td>${s.student_id}</td>
                <td><strong>${escapeHtml(s.full_name)}</strong></td>
                <td>${escapeHtml(s.course)}</td>
                <td>${s.total_marks.toFixed(1)}</td>
                <td>${s.percentage.toFixed(2)}%</td>
                <td><span class="badge-grade ${gradeClass}">${s.grade}</span></td>
                <td><span class="badge-status ${statusClass}">${s.status}</span></td>
                <td>
                    <button class="btn btn-secondary btn-sm" onclick="openMarksheetModal(${s.roll_number})">
                        <i class="fa-solid fa-eye"></i> Marksheet
                    </button>
                </td>
            </tr>
        `;
    });
    tbody.innerHTML = html;
}

function renderMeritList(students) {
    const tbody = document.getElementById('merit-tbody');
    if (!tbody) return;

    if (!students || students.length === 0) {
        tbody.innerHTML = `<tr><td colspan="8" class="text-center" style="padding: 2rem;">No records available to rank.</td></tr>`;
        return;
    }

    const sorted = [...students].sort((a, b) => b.percentage - a.percentage);

    let html = '';
    sorted.forEach((s, idx) => {
        const rank = idx + 1;
        let rankBadge = `<strong>#${rank}</strong>`;
        if (rank === 1) rankBadge = `<span style="color: #d97706; font-size: 1.1rem; font-weight: 800;"><i class="fa-solid fa-trophy"></i> 1st</span>`;
        else if (rank === 2) rankBadge = `<span style="color: #64748b; font-size: 1.05rem; font-weight: 800;"><i class="fa-solid fa-medal"></i> 2nd</span>`;
        else if (rank === 3) rankBadge = `<span style="color: #b45309; font-size: 1.05rem; font-weight: 800;"><i class="fa-solid fa-award"></i> 3rd</span>`;

        const gradeClass = getGradeBadgeClass(s.grade);
        const statusClass = s.status === 'Pass' ? 'status-pass' : 'status-fail';

        html += `
            <tr>
                <td>${rankBadge}</td>
                <td><strong>${s.roll_number}</strong></td>
                <td><strong>${escapeHtml(s.full_name)}</strong></td>
                <td>${escapeHtml(s.course)} (Sem ${s.semester})</td>
                <td>${s.total_marks.toFixed(1)} / 500</td>
                <td><strong>${s.percentage.toFixed(2)}%</strong></td>
                <td><span class="badge-grade ${gradeClass}">${s.grade}</span></td>
                <td><span class="badge-status ${statusClass}">${s.status}</span></td>
            </tr>
        `;
    });
    tbody.innerHTML = html;
}

// ==========================================================================
// 4. Searching & Filtering
// ==========================================================================
let searchTimer = null;
function onSearchInput(query) {
    clearTimeout(searchTimer);
    searchTimer = setTimeout(() => {
        fetchStudents(query);
    }, 250);
}

function handleGlobalSearch(query) {
    switchSection('students');
    const tableInput = document.getElementById('table-search-input');
    if (tableInput) {
        tableInput.value = query;
        onSearchInput(query);
    }
}

function filterRecords() {
    const courseFilter = document.getElementById('filter-course').value;
    const statusFilter = document.getElementById('filter-status').value;

    let filtered = allStudentsCache;
    if (courseFilter) {
        filtered = filtered.filter(s => s.course === courseFilter);
    }
    if (statusFilter) {
        filtered = filtered.filter(s => s.status === statusFilter);
    }

    renderMainTable(filtered);
}

// ==========================================================================
// 5. Add Student & Live Calculation Preview
// ==========================================================================
function calcPreview() {
    const m1 = parseFloat(document.getElementById('add-mark1').value) || 0;
    const m2 = parseFloat(document.getElementById('add-mark2').value) || 0;
    const m3 = parseFloat(document.getElementById('add-mark3').value) || 0;
    const m4 = parseFloat(document.getElementById('add-mark4').value) || 0;
    const m5 = parseFloat(document.getElementById('add-mark5').value) || 0;

    const total = m1 + m2 + m3 + m4 + m5;
    const perc = total / 5.0;
    const hasFailed = [m1, m2, m3, m4, m5].some(m => m < 33.0);

    let status = 'Pass';
    let grade = 'F';

    if (hasFailed) {
        status = 'Fail';
        grade = 'F';
    } else {
        status = 'Pass';
        if (perc >= 90) grade = 'A+';
        else if (perc >= 80) grade = 'A';
        else if (perc >= 70) grade = 'B';
        else if (perc >= 60) grade = 'C';
        else if (perc >= 50) grade = 'D';
        else {
            grade = 'F';
            status = 'Fail';
        }
    }

    document.getElementById('preview-total').textContent = `${total.toFixed(1)} / 500`;
    document.getElementById('preview-perc').textContent = `${perc.toFixed(2)}%`;

    const gradeEl = document.getElementById('preview-grade');
    gradeEl.textContent = grade;
    gradeEl.className = `badge-grade ${getGradeBadgeClass(grade)}`;

    const statusEl = document.getElementById('preview-status');
    statusEl.textContent = status;
    statusEl.className = `badge-status ${status === 'Pass' ? 'status-pass' : 'status-fail'}`;
}

async function submitAddStudent(e) {
    e.preventDefault();

    const studentData = {
        student_id: document.getElementById('add-student-id').value.trim(),
        roll_number: parseInt(document.getElementById('add-roll-number').value, 10),
        full_name: document.getElementById('add-full-name').value.trim(),
        age: parseInt(document.getElementById('add-age').value, 10),
        course: document.getElementById('add-course').value.trim(),
        semester: parseInt(document.getElementById('add-semester').value, 10),
        phone_number: document.getElementById('add-phone').value.trim(),
        marks: [
            parseFloat(document.getElementById('add-mark1').value) || 0,
            parseFloat(document.getElementById('add-mark2').value) || 0,
            parseFloat(document.getElementById('add-mark3').value) || 0,
            parseFloat(document.getElementById('add-mark4').value) || 0,
            parseFloat(document.getElementById('add-mark5').value) || 0
        ]
    };

    const submitBtn = document.getElementById('btn-save-student');
    submitBtn.disabled = true;
    submitBtn.innerHTML = '<i class="fa-solid fa-spinner fa-spin"></i> Saving to SQLite...';

    try {
        const response = await fetch('/api/students', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(studentData)
        });

        const result = await response.json();

        if (response.ok && result.success) {
            showToast(`Student #${studentData.roll_number} added successfully!`, 'success');
            document.getElementById('add-student-form').reset();
            calcPreview();
            fetchDashboardStats();
            fetchStudents();
            switchSection('students');
        } else {
            showToast(result.error || 'Failed to add student record', 'error');
        }
    } catch (err) {
        console.error('Error adding student:', err);
        showToast('Network error while saving student', 'error');
    } finally {
        submitBtn.disabled = false;
        submitBtn.innerHTML = '<i class="fa-solid fa-floppy-disk"></i> Save Student to SQLite';
    }
}

// ==========================================================================
// 6. Edit Student Modal & Submission
// ==========================================================================
async function openEditModal(rollNumber) {
    try {
        const response = await fetch(`/api/students/${rollNumber}`);
        if (!response.ok) throw new Error('Student not found');
        const s = await response.json();

        document.getElementById('edit-original-roll').value = s.roll_number;
        document.getElementById('edit-student-id').value = s.student_id;
        document.getElementById('edit-roll-number').value = s.roll_number;
        document.getElementById('edit-full-name').value = s.full_name;
        document.getElementById('edit-age').value = s.age;
        document.getElementById('edit-course').value = s.course;
        document.getElementById('edit-semester').value = s.semester;
        document.getElementById('edit-phone').value = s.phone_number;

        document.getElementById('edit-mark1').value = s.marks[0];
        document.getElementById('edit-mark2').value = s.marks[1];
        document.getElementById('edit-mark3').value = s.marks[2];
        document.getElementById('edit-mark4').value = s.marks[3];
        document.getElementById('edit-mark5').value = s.marks[4];

        document.getElementById('edit-modal').classList.add('show');
    } catch (err) {
        console.error('Error opening edit modal:', err);
        showToast('Could not fetch student details for editing', 'error');
    }
}

function closeEditModal() {
    document.getElementById('edit-modal').classList.remove('show');
}

async function submitEditStudent(e) {
    e.preventDefault();
    const originalRoll = document.getElementById('edit-original-roll').value;

    const updatedData = {
        student_id: document.getElementById('edit-student-id').value.trim(),
        roll_number: parseInt(document.getElementById('edit-roll-number').value, 10),
        full_name: document.getElementById('edit-full-name').value.trim(),
        age: parseInt(document.getElementById('edit-age').value, 10),
        course: document.getElementById('edit-course').value.trim(),
        semester: parseInt(document.getElementById('edit-semester').value, 10),
        phone_number: document.getElementById('edit-phone').value.trim(),
        marks: [
            parseFloat(document.getElementById('edit-mark1').value) || 0,
            parseFloat(document.getElementById('edit-mark2').value) || 0,
            parseFloat(document.getElementById('edit-mark3').value) || 0,
            parseFloat(document.getElementById('edit-mark4').value) || 0,
            parseFloat(document.getElementById('edit-mark5').value) || 0
        ]
    };

    try {
        const response = await fetch(`/api/students/${originalRoll}`, {
            method: 'PUT',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(updatedData)
        });

        const result = await response.json();

        if (response.ok && result.success) {
            showToast('Student record updated successfully!', 'success');
            closeEditModal();
            fetchDashboardStats();
            fetchStudents();
        } else {
            showToast(result.error || 'Failed to update student record', 'error');
        }
    } catch (err) {
        console.error('Error updating student:', err);
        showToast('Network error while updating record', 'error');
    }
}

// ==========================================================================
// 7. Delete Student Record
// ==========================================================================
async function confirmDeleteStudent(rollNumber, name) {
    if (!confirm(`Are you sure you want to permanently delete student:\n\n${name} (Roll No: ${rollNumber})?\n\nThis action cannot be undone.`)) {
        return;
    }

    try {
        const response = await fetch(`/api/students/${rollNumber}`, {
            method: 'DELETE'
        });

        const result = await response.json();

        if (response.ok && result.success) {
            showToast(`Student #${rollNumber} deleted successfully`, 'success');
            fetchDashboardStats();
            fetchStudents();
        } else {
            showToast(result.error || 'Failed to delete student record', 'error');
        }
    } catch (err) {
        console.error('Error deleting student:', err);
        showToast('Network error while deleting record', 'error');
    }
}

// ==========================================================================
// 8. Official Marksheet Modal
// ==========================================================================
async function openMarksheetModal(rollNumber) {
    try {
        const response = await fetch(`/api/students/${rollNumber}`);
        if (!response.ok) throw new Error('Student not found');
        const s = await response.json();

        const gradeClass = getGradeBadgeClass(s.grade);
        const statusColor = s.status === 'Pass' ? '#059669' : '#dc2626';

        let subjectRows = '';
        s.marks.forEach((mark, i) => {
            const isFail = mark < 33.0;
            subjectRows += `
                <tr>
                    <td style="text-align: center;">${i + 1}</td>
                    <td>${SUBJECT_NAMES[i]}</td>
                    <td style="text-align: center;">100</td>
                    <td style="text-align: center; font-weight: 700; ${isFail ? 'color: #dc2626;' : ''}">
                        ${mark.toFixed(1)} ${isFail ? '<span title="Failed (<33)">*</span>' : ''}
                    </td>
                    <td style="text-align: center; font-weight: 600; color: ${isFail ? '#dc2626' : '#059669'};">
                        ${isFail ? 'FAIL' : 'PASS'}
                    </td>
                </tr>
            `;
        });

        const marksheetHtml = `
            <div class="marksheet-doc">
                <div class="marksheet-header">
                    <h2>DEPARTMENT OF COMPUTER APPLICATIONS</h2>
                    <p>OFFICIAL STUDENT ACADEMIC MARKSHEET</p>
                    <p style="font-size: 0.8rem; margin-top: 0.25rem;">Academic Session: 2024-2025 &bull; Semester Examination</p>
                </div>

                <div class="marksheet-info-grid">
                    <div><strong>Student ID:</strong> ${escapeHtml(s.student_id)}</div>
                    <div><strong>Roll Number:</strong> ${s.roll_number}</div>
                    <div><strong>Student Name:</strong> ${escapeHtml(s.full_name)}</div>
                    <div><strong>Course & Sem:</strong> ${escapeHtml(s.course)} (Semester ${s.semester})</div>
                    <div><strong>Phone:</strong> ${escapeHtml(s.phone_number)}</div>
                    <div><strong>Age:</strong> ${s.age} years</div>
                </div>

                <table class="marksheet-table">
                    <thead>
                        <tr>
                            <th style="width: 50px; text-align: center;">Sr.</th>
                            <th>Subject Name</th>
                            <th style="width: 100px; text-align: center;">Max Marks</th>
                            <th style="width: 120px; text-align: center;">Marks Obtained</th>
                            <th style="width: 90px; text-align: center;">Status</th>
                        </tr>
                    </thead>
                    <tbody>
                        ${subjectRows}
                    </tbody>
                </table>

                <div class="marksheet-summary">
                    <div>
                        <p><strong>Total Marks Obtained:</strong> ${s.total_marks.toFixed(1)} / 500</p>
                        <p><strong>Aggregate Percentage:</strong> ${s.percentage.toFixed(2)}%</p>
                    </div>
                    <div style="text-align: right;">
                        <p><strong>Final Grade:</strong> <span class="badge-grade ${gradeClass}">${s.grade}</span></p>
                        <p><strong>Academic Result:</strong> <strong style="color: ${statusColor}; font-size: 1.1rem;">${s.status.toUpperCase()}</strong></p>
                    </div>
                </div>

                <div style="margin-top: 2.5rem; display: flex; justify-content: space-between; font-size: 0.85rem; color: #475569;">
                    <div>Date of Issue: ${new Date().toLocaleDateString()}</div>
                    <div style="text-align: center; border-top: 1px solid #475569; width: 160px; padding-top: 0.35rem;">Controller of Examination</div>
                </div>
            </div>
        `;

        document.getElementById('marksheet-content').innerHTML = marksheetHtml;
        document.getElementById('marksheet-modal').classList.add('show');
    } catch (err) {
        console.error('Error loading marksheet:', err);
        showToast('Could not load official marksheet', 'error');
    }
}

function closeMarksheetModal() {
    document.getElementById('marksheet-modal').classList.remove('show');
}

// ==========================================================================
// 9. Toast Notification Helper
// ==========================================================================
function showToast(message, type = 'success') {
    const container = document.getElementById('toast-container');
    const toast = document.createElement('div');
    toast.className = `toast toast-${type}`;
    const icon = type === 'success' ? '<i class="fa-solid fa-circle-check"></i>' : '<i class="fa-solid fa-triangle-exclamation"></i>';
    toast.innerHTML = `${icon} <span>${escapeHtml(message)}</span>`;

    container.appendChild(toast);

    setTimeout(() => {
        toast.style.animation = 'toastSlide 0.3s reverse forwards';
        setTimeout(() => toast.remove(), 300);
    }, 3500);
}

function escapeHtml(str) {
    if (!str) return '';
    return String(str)
        .replace(/&/g, '&amp;')
        .replace(/</g, '&lt;')
        .replace(/>/g, '&gt;')
        .replace(/"/g, '&quot;')
        .replace(/'/g, '&#039;');
}
